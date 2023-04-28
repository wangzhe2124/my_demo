#shader vertex
#version 420 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
#define SPLITNUM 4
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 DirlightCSMMatrix[SPLITNUM];
uniform mat4 SpotlightSpaceMatrix;
out VS_OUT{
 vec3 Normal;
 vec3 FragPos;
 vec2 TexCoord;
 vec4 DirLightCSMSpace[SPLITNUM];
 vec4 SpotLightSpace;
 mat3 TBN;
}vs_out;
void main()
{	
	gl_Position = projection * view * model * vec4(aPosition, 1.0f);
	vs_out.FragPos = vec3(model * vec4(aPosition, 1.0));
	vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;//法线变换到世界空间，应设置uniform在主程序执行提升效率	
	vs_out.TexCoord = aTexCoord;
	for(int i = 0;i < SPLITNUM;i++)
	{ 
		vs_out.DirLightCSMSpace[i] = DirlightCSMMatrix[i] * vec4(vs_out.FragPos, 1.0);
	}	
	vs_out.SpotLightSpace = SpotlightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
	//切线空间
	vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
	//T = normalize(T - dot(T, N) * N);	
	vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
	//vec3 B = cross(T, N);
	vs_out.TBN = mat3(T, B, N);//如果不转置，方向变反
};

#shader fragment
#version 420 core
layout(location = 0) out vec4 color;
#define SPLITNUM 4
struct Material//材质信息
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	sampler2D texture_height1;
	float shininess;
};
struct DirLight//平行光
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
	float LightIntensity;
	float far_plane;
	float near_plane;
};
struct PointLight//点光源
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
	float constant;
	float linear;
	float quadratic;
	float LightIntensity;
	float far_plane;
	float near_plane;
};
struct SpotLight//聚光
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
	vec3 direction;
	float constant;
	float linear;
	float quadratic;
	float LightIntensity;
	float inner_CutOff;
	float outer_CutOff;
	float far_plane;
	float near_plane;
};
struct Camera
{
	vec3 viewPos;
	float near_plane;
	float far_plane;
};
uniform Camera camera;
uniform Material material;
uniform DirLight dirlight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointlight[NR_POINT_LIGHTS];
uniform SpotLight spotlight;
uniform vec3 objectColor;
uniform samplerCube skybox;
uniform bool  blinn_phong;
uniform bool use_NormalMap;
uniform bool use_HeightMap;
struct ShadowMap
{
	sampler2DArray DirCSM;
	sampler2D csm_map[SPLITNUM];

	samplerCube PointShadow[4];
	sampler2D SpotShadow;
};
uniform ShadowMap shadowMap;
uniform float far_plane;
uniform float near_plane;
uniform float split_distance[SPLITNUM];
uniform float z_distance[SPLITNUM];
in VS_OUT{
vec3 Normal;//注意顺序要与顶点着色器输出一致
vec3 FragPos;
vec2 TexCoord;
vec4 DirLightCSMSpace[SPLITNUM];
vec4 SpotLightSpace;
mat3 TBN;
}fs_in;
//透视的深度回到线性
float LinearizeDepth(float depth ,float near_plane, float far_plane)
{
	float z = depth * 2.0 - 1.0; // Back to NDC ，0到1变成-1到1
	return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}
vec3 CalcDirLight(vec3 normal, vec2 Texcoord);
vec3 CalcPointLight(int i, vec3 normal, vec2 Texcoord);
vec3 CalcSpotLight(vec3 normal, vec2 Texcoord);

float DirShadowCalculation(vec3 normal);
float PointShadowCalculation(int i, vec3 normal);
float SpotShadowCalculation(vec3 normal);

//高度（视差）贴图计算位移的纹理坐标
vec2 ParallaxMapping(vec2 texCoord, vec3 viewDir, vec3 normal)
{
	float height_scale = 0.1;
	// number of depth layers
	const float numLayers = 20;
	// calculate the size of each layer
	float layerDepth = 1.0 / numLayers;
	// depth of current layer
	float currentLayerDepth = 0.0;
	// the amount to shift the texture coordinates per layer (from vector P)
	vec2 P = viewDir.xy * height_scale;
	vec2 deltaTexCoords = P / numLayers;
	// get initial values
	vec2  currentTexCoords = texCoord;
	float currentDepthMapValue = texture(material.texture_height1, currentTexCoords).r;

	while (currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = texture(material.texture_height1, currentTexCoords).r;
		// get depth of next layer
		currentLayerDepth += layerDepth;
	}
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// get depth after and before collision for linear interpolation
	float afterDepth = currentLayerDepth - currentDepthMapValue;
	float beforeDepth = texture(material.texture_height1, prevTexCoords).r - currentLayerDepth + layerDepth;

	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth + beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;

}
float random(vec3 seed, int i) {
	vec4 seed4 = vec4(seed, i);
	float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
	return fract(sin(dot_product) * 43758.5453);
}
void main()
{
	vec3 viewDir = normalize(camera.viewPos - fs_in.FragPos);
	//选择是否使用高度贴图的纹理坐标
	vec2 Texcoord = ParallaxMapping(fs_in.TexCoord, viewDir, fs_in.Normal);
	Texcoord = use_HeightMap == true ? Texcoord : fs_in.TexCoord;
	//选择是否使用法线贴图
	vec3 Normal = texture(material.texture_normal1, fs_in.TexCoord).rgb;

	Normal = normalize(fs_in.TBN * normalize(Normal));
	vec3 normal = use_NormalMap  == true ? Normal : normalize(fs_in.Normal);

	float ratio = 1.00 / 1.52;//玻璃折射率
	vec3 I = -viewDir;
	vec3 R = reflect(I, normal);//反射方向
	vec3 RF = refract(I, normal, ratio);//折射方向

	vec3 result = CalcDirLight(normal, Texcoord);
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		result += CalcPointLight(i, normal, Texcoord);
	}
	result += CalcSpotLight(normal, Texcoord);
	result = result * objectColor;
	color = vec4(pow(result, vec3(1/2.2)), 1.0); //+ vec4(texture(skybox, RF).rgb, 1.0);
};
vec3 CalcDirLight(vec3 normal, vec2 Texcoord)
{	
	vec3 viewDir = normalize(camera.viewPos - fs_in.FragPos);
	vec3 lightDir = normalize(dirlight.direction);
	float diff = max(dot(normal, -lightDir), 0.0);//漫反射
	//高光
	float spec;
	if (blinn_phong)
	{
		vec3 halfwayDir = normalize(-lightDir + viewDir);
		spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	}
	else
	{
		vec3 reflectDir = reflect(lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	}
	float shadow = DirShadowCalculation(normal);//计算是否在阴影中
	vec3 ambient = dirlight.ambient * vec3(texture(material.texture_diffuse1, Texcoord));
	vec3 diffuse = dirlight.diffuse * diff * vec3(texture(material.texture_diffuse1, Texcoord));
	vec3 specular = dirlight.specular * spec * vec3(texture(material.texture_specular1, Texcoord));
	return (ambient * 0.1 + (diffuse + specular) * (1 - shadow)) * dirlight.LightIntensity;
}
vec3 CalcPointLight(int i, vec3 normal, vec2 Texcoord)
{	
	PointLight pt = pointlight[i];
	vec3 fragPos = fs_in.FragPos;
	vec3 viewDir = normalize(camera.viewPos - fragPos);

	vec3 lightDir = normalize(fragPos - pt.position);
	float diff = max(dot(normal, -lightDir), 0.0);//漫反射
	//高光
	float spec;
	if (blinn_phong)
	{
		vec3 halfwayDir = normalize(-lightDir + viewDir);
		spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	}
	else
	{
		vec3 reflectDir = reflect(lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	}
	float shadow = PointShadowCalculation(i, normal);//计算是否在阴影中

	float distance = length(pt.position - fragPos);//点光源距离
	float attenuation = pt.LightIntensity / (pt.constant + pt.linear * distance + pt.quadratic * (distance * distance));
	vec3 ambient = pt.ambient * vec3(texture(material.texture_diffuse1, Texcoord));
	vec3 diffuse = pt.diffuse * diff * vec3(texture(material.texture_diffuse1, Texcoord));
	vec3 specular = pt.specular * spec * vec3(texture(material.texture_specular1, Texcoord));

	return (ambient * 0.1 + (diffuse + specular) * (1 - shadow)) * attenuation;
}
vec3 CalcSpotLight(vec3 normal, vec2 Texcoord)
{
	vec3 fragPos = fs_in.FragPos;
	vec3 viewDir = normalize(camera.viewPos - fragPos);

	vec3 lightDir = normalize(fragPos - spotlight.position);
	float diff = max(dot(normal, -lightDir), 0.0);//漫反射
	//高光
	float spec;
	if (blinn_phong)
	{
		vec3 halfwayDir = normalize(-lightDir + viewDir);
		spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	}
	else
	{
		vec3 reflectDir = reflect(lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	}
	float shadow = SpotShadowCalculation(normal);//计算是否在阴影中
	float distance = length(spotlight.position - fragPos);//距离
	float attenuation = spotlight.LightIntensity / (spotlight.constant + spotlight.linear * distance + spotlight.quadratic * (distance * distance));

	float theta = dot(lightDir, normalize(spotlight.direction));//聚光
	float epsilon = spotlight.inner_CutOff - spotlight.outer_CutOff;
	float intensity = clamp((theta - spotlight.outer_CutOff) / epsilon, 0.0, 1.0);
	vec3 ambient = spotlight.ambient * vec3(texture(material.texture_diffuse1, Texcoord));
	vec3 diffuse = spotlight.diffuse * diff * vec3(texture(material.texture_diffuse1, Texcoord));
	vec3 specular = spotlight.specular * spec * vec3(texture(material.texture_specular1, Texcoord));

	diffuse *= intensity;
	specular *= intensity;
	return (ambient * 0.1 + (diffuse + specular) * (1- shadow)) * attenuation;
}
float DirShadowCalculation(vec3 normal)
{
	vec3 fragPosition = fs_in.FragPos;

	float frag_distance = LinearizeDepth(gl_FragCoord.z, camera.near_plane, camera.far_plane);
	int index = 0;
	if (frag_distance < split_distance[0]) {
		index = 0;
	}
	else if (frag_distance < split_distance[1]) {
		index = 1;
	}
	else if (frag_distance < split_distance[2]) {
		index = 2;
	}
	else if (frag_distance < split_distance[3]) {
		index = 3;
	}
	else
		index = SPLITNUM;
	vec3 lightDir = normalize(dirlight.direction);
	vec3 projCoords = fs_in.DirLightCSMSpace[index].xyz;
	projCoords = projCoords * 0.5 + 0.5;//-1到1 变成0到1
	float closestDepth = texture(shadowMap.csm_map[index], projCoords.xy).r;//距离光源位置最近的深度
	float currentDepth = projCoords.z;//光源视角下当前片元的深度
	float costheta = max(dot(normal, -lightDir),0);
	float theta = clamp(acos(costheta), 0.0, 89 * 3.14 / 180);
	float texSize = 1.0 / textureSize(shadowMap.csm_map[index], 0).x;
	float view_distance = clamp(length(camera.viewPos - fragPosition), 1.0, 2.5);

	float bias = 1.0 *  texSize *tan(theta) / z_distance[index] * view_distance;//根据视锥体z高度调整bias
	float shadow = 0.0;
	/*vec2 poissonDisk[25];
	int L = 2;
	for (int t = -L; t <= L; t++)
	{
		for (int j = -L; j <= L; j++)
		{
			int temp = (t + L) * (2 * L + 1) + j + L;
			poissonDisk[temp] = vec2(t,j);
		}
	}
	//float texSize = 1.5f / z_distance[index] / textureSize(shadowMap.csm_map[index], 0).x; 
	if (currentDepth - bias > closestDepth)
	{
		for (int i = 0; i < 25; i++)
		{
			//int offset = int(25.0 * random(fs_in.TexCoord.xyy, i)) % 25;
			float pcfDepth = texture(shadowMap.csm_map[index], projCoords.xy + poissonDisk[i] * texSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;

		}
		shadow /= 25;
	}*/	
	shadow += currentDepth  - bias> closestDepth ? 1.0 : 0.0;
	return shadow;
}

float PointShadowCalculation(int i, vec3 normal)
{	
	vec3 fragPosition = fs_in.FragPos;
	float view_distance = clamp(length(camera.viewPos - fragPosition), 1.0, 1.1);
	vec3 lightDir = normalize(fragPosition - pointlight[i].position);
	float closestDepth = texture(shadowMap.PointShadow[i], lightDir).r;
	closestDepth *= pointlight[i].far_plane;
	float currentDepth = length(fragPosition - pointlight[i].position);
	float costheta = max(dot(normal, -lightDir),0);
	float theta = clamp(acos(costheta), 0.0, 89 * 3.14/180);
	float texSize = 1.0 / textureSize(shadowMap.PointShadow[i], 0).x;

	float bias =  texSize  * tan(theta) * pointlight[i].far_plane * view_distance;
	//bias = clamp(bias, 0.0, 0.1 * pointlight[i].far_plane);
	float shadow = 0.0;
	if (currentDepth - bias > closestDepth)
	{		
		vec3 up = vec3(0.0, 0.0, 1.0);
		vec3 offset1 = normalize(cross(up, lightDir));
		vec3 offset2 = normalize(cross(offset1, lightDir));
		float radius = 4 * (currentDepth - bias - closestDepth) / closestDepth / textureSize(shadowMap.PointShadow[i], 0).x;
		int sampleNum = 25;
		vec3 poissonDisk[25];
		int L = 2;
		for (int t = -L; t <= L; t++)
		{
			vec3 offset = offset1 + t * offset2;
			for (int j = -L; j <= L; j++)
			{
				int temp = (t + L) * (2 * L + 1) + j + L;
				poissonDisk[temp] = radius * j * offset;
			}
		}
		for (int t = 0; t < sampleNum; t++)
		{
			int index = int(25.0 * random(fs_in.TexCoord.xyy, t)) % 25;
			float pcfDepth = texture(shadowMap.PointShadow[i], lightDir + 3 *poissonDisk[t]).r;
			pcfDepth *= pointlight[i].far_plane;  // Undo mapping [0;1]
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
		shadow /= sampleNum;				
	}	
	//shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
	return shadow;
}

float SpotShadowCalculation(vec3 normal)
{	
	vec3 fragPosition = fs_in.FragPos;
	vec3 lightDir = normalize(fragPosition - spotlight.position);
	vec3 projCoords = fs_in.SpotLightSpace.xyz / fs_in.SpotLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;//-1到1 变成0到1
	float closestDepth = texture(shadowMap.SpotShadow, projCoords.xy).r;//距离光源位置最近的深度
	float currentDepth = projCoords.z;//光源视角下当前片元的深度
	float shadow = currentDepth - 0.001 > closestDepth ? 1.0 : 0.0;
	/*float shadow = 0.0;
	float texelSize = 1.0 / textureSize(shadowMap.SpotShadow, 0).x;
	vec2 poissonDisk[25];
	int L = 2;
	for (int t = -L; t <= L; t++)
	{
		for (int j = -L; j <= L; j++)
		{
			int temp = (t + L) * (2 * L + 1) + j + L;
			poissonDisk[temp] = texelSize * vec2(j, t);
		}
	}

	if (currentDepth - bias > closestDepth)
	{
		for (int x = 0; x < 25; x++)
		{
			int index = int(25.0 * random(fs_in.TexCoord.xyy, x)) % 25;
			float pcfDepth = texture(shadowMap.SpotShadow, projCoords.xy + poissonDisk[index] * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
		shadow /= 25.0;		
	}*/
	return shadow;
}