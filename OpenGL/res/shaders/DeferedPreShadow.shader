#shader vertex
#version 460 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;

#define SPLITNUM 4
out VS_OUT{
 vec2 TexCoord;
}vs_out;
void main()
{
	gl_Position = vec4(Position, 1.0f);
	vs_out.TexCoord = TexCoord;
};

#shader fragment
#version 460 core
layout(location = 0) out vec2 DirSpotPreShadow;
layout(location = 1) out vec4 PointPreShadow;
//layout(location = 2) out vec4 SpotPreShadow;
#define SPLITNUM 4
#define POINT_LIGHTS_NUM 4
const float PI = 3.14159265359;

struct Material//材质信息
{
	float metallic;
	float roughness;
};
struct DirLight//平行光
{
	vec3 color;
	vec3 direction;
	float LightIntensity;
	float far_plane;
	float near_plane;
};
struct PointLight//点光源
{
	vec3 color;
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
	vec3 color;
	vec3 position;
	vec3 direction;
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
	sampler2D Depth;
};
uniform Camera camera;

uniform DirLight dirlight;

uniform PointLight pointlight[POINT_LIGHTS_NUM];
uniform SpotLight spotlight;

in VS_OUT{
vec2 TexCoord;
}fs_in;

float DirShadowCalculation(vec3 normal, vec3 FragPos);
float PointShadowCalculation(int i, vec3 normal, vec3 FragPos);
float SpotShadowCalculation(vec3 normal, vec3 FragPos);

//透视的深度回到线性
float LinearizeDepth(float depth, float near_plane, float far_plane)
{
	float z = depth * 2.0 - 1.0; // Back to NDC ，0到1变成-1到1
	return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform samplerCube EnvLight;
struct ShadowMap
{
	sampler2D global_dirshadow;
	sampler2D csm_map[SPLITNUM];
	samplerCube PointShadow[4];
	sampler2D SpotShadow;
};
uniform ShadowMap shadowMap;
uniform float far_plane;
uniform float near_plane;
uniform float split_distance[SPLITNUM];
uniform float z_distance[SPLITNUM];
uniform float xy_distance[SPLITNUM];

uniform mat4 DirlightCSMMatrix[SPLITNUM];
uniform mat4 SpotlightSpaceMatrix;
uniform mat4 globallightSpaceMatrix;

float random(vec3 seed, int i) {
	vec4 seed4 = vec4(seed, i);
	float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
	return fract(sin(dot_product) * 43758.5453);
}
void main()
{
	vec2 Texcoord = fs_in.TexCoord;
	vec3 FragPos = texture(gPosition, Texcoord).rgb;
	vec3 normal = texture(gNormal, Texcoord).rgb;
	DirSpotPreShadow = vec2(DirShadowCalculation(normal, FragPos), SpotShadowCalculation(normal, FragPos));
	PointPreShadow = vec4(
		PointShadowCalculation(0, normal, FragPos),
		PointShadowCalculation(1, normal, FragPos),
		PointShadowCalculation(2, normal, FragPos),
		PointShadowCalculation(3, normal, FragPos));
	//SpotPreShadow = vec4(SpotShadowCalculation( normal,FragPos), 0.0, 0.0, 1.0);
};
uniform float sun_sm_bias;
uniform bool sun_pcf;
uniform float sun_pcf_radius;
float DirShadowCalculation(vec3 normal, vec3 FragPos)
{
	vec4 DirLightCSMSpace[SPLITNUM];
	for (int i = 0; i < SPLITNUM; i++)
	{
		DirLightCSMSpace[i] = DirlightCSMMatrix[i] * vec4(FragPos, 1.0);
	}
	float camera_depth = texture(camera.Depth, fs_in.TexCoord).x;
	float frag_distance = LinearizeDepth(camera_depth, camera.near_plane, camera.far_plane);
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
		return 0;
	vec3 lightDir = normalize(dirlight.direction);
	vec3 projCoords = DirLightCSMSpace[index].xyz;
	projCoords = projCoords * 0.5 + 0.5;//-1到1 变成0到1

	float closestDepth = texture(shadowMap.csm_map[index], projCoords.xy).r;//距离光源位置最近的深度
	float currentDepth = projCoords.z;//光源视角下当前片元的深度
	float costheta = max(dot(normal, -lightDir), 0);
	float theta = acos(costheta);
	float texSize = 1.0 / textureSize(shadowMap.csm_map[index], 0).x ;
	float bias = sun_sm_bias * texSize * tan(theta);//根据视锥体z高度调整bias
	float shadow = 0.0;

/*	float mean = texture(shadowMap.csm_map[index], projCoords.xy, 3).r;
	float square = texture(shadowMap.csm_map[index], projCoords.xy, 3).g;
	float var = square - mean * mean;
	float prob = 1 - var / (var + pow(closestDepth - mean, 2));
	float blocker = (mean - prob * currentDepth) / (1 - prob);*///vssm
	float soft_shadow = 1.0f + (currentDepth - closestDepth) * 1000;
	if (sun_pcf)
	{
		vec2 poissonDisk[25];
		int L = 2;
		for (int t = -L; t <= L; t++)
		{
			for (int j = -L; j <= L; j++)
			{
				int temp = (t + L) * (2 * L + 1) + j + L;
				poissonDisk[temp] = soft_shadow * sun_pcf_radius * texSize * vec2(j, t);
			}
		}
		for (int x = 0; x < 25; x++)
		{
			int random_index = int(25.0 * random(fs_in.TexCoord.xyy, x)) % 25;
			float pcfDepth = texture(shadowMap.csm_map[index], projCoords.xy + poissonDisk[random_index] / pow(xy_distance[index], 0.5)).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
		shadow /= 25;		
	}
	else
		shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	return shadow;
}
uniform float point_sm_radius;
uniform bool point_sm_pcf;
float PointShadowCalculation(int i, vec3 normal, vec3 FragPos)
{
	PointLight pt = pointlight[i];
	float view_distance = clamp(length(camera.viewPos - FragPos), 0.3, 0.5);
	vec3 lightDir = normalize(FragPos - pt.position);
	float closestDepth = texture(shadowMap.PointShadow[i], lightDir).r;
	closestDepth *= pt.far_plane;
	float currentDepth = length(FragPos - pt.position);
	float costheta = max(dot(normal, -lightDir), 0);
	float theta = clamp(acos(costheta), 0.0, 89 * 3.14 / 180);
	float texSize = 1.0 / textureSize(shadowMap.PointShadow[i], 0).x;
	float bias = texSize * tan(theta) * pt.far_plane * view_distance;
	float shadow = 0.0;
	float distance = length(pt.position - FragPos);
	if (point_sm_pcf && (distance < pt.far_plane))
	{
		vec3 up = vec3(0.0, 0.0, 1.0);
		vec3 offset1 = normalize(cross(up, lightDir));
		vec3 offset2 = normalize(cross(offset1, lightDir));
		//float radius = 40.0 / textureSize(shadowMap.PointShadow[i], 0).x;
		int sampleNum = 25;
		vec3 poissonDisk[25];
		int L = 2;
		float pcf_radius = point_sm_radius / 1000.0f;
		for (int t = -L; t <= L; t++)
		{
			vec3 offset = offset1 + t * offset2;
			for (int j = -L; j <= L; j++)
			{
				int temp = (t + L) * (2 * L + 1) + j + L;
				poissonDisk[temp] = pcf_radius * j * offset;
			}
		}
		for (int t = 0; t < sampleNum; t++)
		{
			int index = int(25.0 * random(fs_in.TexCoord.xyy, t)) % 25;
			float pcfDepth = texture(shadowMap.PointShadow[i], lightDir + poissonDisk[index]).r;
			pcfDepth *= pt.far_plane;  // Undo mapping [0;1]
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
		shadow /= float(sampleNum);
	}
	else
		shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
	return shadow;
}

float SpotShadowCalculation(vec3 normal, vec3 FragPos)
{
	vec4 SpotLightSpace;
	SpotLightSpace = SpotlightSpaceMatrix * vec4(FragPos, 1.0);
	vec3 projCoords = SpotLightSpace.xyz / SpotLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;//-1到1 变成0到1
	float closestDepth = texture(shadowMap.SpotShadow, projCoords.xy).r;//距离光源位置最近的深度
	float currentDepth = projCoords.z;//光源视角下当前片元的深度
	float shadow = currentDepth - 0.01 > closestDepth ? 1.0 : 0.0;
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
