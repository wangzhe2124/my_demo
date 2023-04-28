#shader vertex
#version 420 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in mat4 instanceMatrix;
uniform mat4 projection;
uniform mat4 view;

out VS_OUT{
 vec3 Normal;
 vec3 FragPos;
 vec2 TexCoord;
}vs_out;
void main()
{
	vs_out.FragPos = vec3(instanceMatrix * vec4(aPosition, 1.0));
	vs_out.Normal = mat3(transpose(inverse(instanceMatrix))) * aNormal;//法线变换到世界空间，应设置uniform在主程序执行提升效率
	gl_Position = projection * view * instanceMatrix * vec4(aPosition, 1.0f);
	vs_out.TexCoord = aTexCoord;

};

#shader fragment
#version 420 core
layout(location = 0) out vec4 color;
struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	float shininess;

};
struct DirLight//平行光
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
	float LightIntensity;
};
vec3 CalcDirLight(DirLight dirlight, vec3 normal, vec3 viewDir);
struct PointLight
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
	float constant;
	float linear;
	float quadratic;
	float LightIntensity;
};
vec3 CalcPointLight(PointLight pointlight, vec3 normal, vec3 fragPos, vec3 viewDir);
struct SpotLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
	vec3 direction;
	float constant;
	float linear;
	float quadratic;
	float LightIntensity;

	float inner_CutOff;//聚光spotlight
	float outer_CutOff;
};
vec3 CalcSpotLight(SpotLight spotlight, vec3 normal, vec3 fragPos, vec3 viewDir);

uniform Material material;
uniform DirLight dirlight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointlight[NR_POINT_LIGHTS];
uniform SpotLight spotlight;
uniform vec3 objectColor;
uniform vec3 viewPos;
uniform samplerCube skybox;
uniform float alpha;

in VS_OUT{
vec3 Normal;//注意顺序要与顶点着色器输出一致
vec3 FragPos;
vec2 TexCoord;
}fs_in;
void main()
{
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 normal = normalize(fs_in.Normal);//normalize(vec3(texture(material.texture_normal1, fs_in.TexCoord)));
	float ratio = 1.00 / 1.52;//玻璃折射率
	vec3 I = -viewDir;
	vec3 R = reflect(I, normal);//反射方向
	vec3 RF = refract(I, normal, ratio);//折射方向

	vec3 result = CalcDirLight(dirlight, normal, viewDir);
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		result += CalcPointLight(pointlight[i], normal, fs_in.FragPos, viewDir);
	}
	result += CalcSpotLight(spotlight, normal, fs_in.FragPos, viewDir);
	result = pow(result * objectColor, vec3(1.0 / 2.2));
	color = vec4(result, 1.0); //+ vec4(texture(skybox, RF).rgb, 1.0);
};
vec3 CalcDirLight(DirLight dirlight, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-dirlight.direction);
	float diff = max(dot(normal, lightDir), 0.0);//漫反射
	vec3 reflectDir = reflect(-lightDir, normal);//高光
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 ambient = dirlight.ambient * vec3(texture(material.texture_diffuse1, fs_in.TexCoord));
	vec3 diffuse = dirlight.diffuse * diff * vec3(texture(material.texture_diffuse1, fs_in.TexCoord));
	vec3 specular = dirlight.specular * spec * vec3(texture(material.texture_specular1, fs_in.TexCoord));
	return (ambient * 0.1 + diffuse + specular) * dirlight.LightIntensity;
}
vec3 CalcPointLight(PointLight pointlight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(pointlight.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);//漫反射
	vec3 reflectDir = reflect(-lightDir, normal);//高光
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	float distance = length(pointlight.position - fragPos);//点光源距离
	float attenuation = pointlight.LightIntensity / (pointlight.constant + pointlight.linear * distance + pointlight.quadratic * (distance * distance));
	vec3 ambient = pointlight.ambient * vec3(texture(material.texture_diffuse1, fs_in.TexCoord));
	vec3 diffuse = pointlight.diffuse * diff * vec3(texture(material.texture_diffuse1, fs_in.TexCoord));
	vec3 specular = pointlight.specular * spec * vec3(texture(material.texture_specular1, fs_in.TexCoord));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient * 0.1 + diffuse + specular);
}
vec3 CalcSpotLight(SpotLight spotlight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(spotlight.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);//漫反射
	vec3 reflectDir = reflect(-lightDir, normal);//高光
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	float distance = length(spotlight.position - fragPos);//距离
	float attenuation = spotlight.LightIntensity / (spotlight.constant + spotlight.linear * distance + spotlight.quadratic * (distance * distance));

	float theta = dot(lightDir, normalize(-spotlight.direction));//聚光
	float epsilon = spotlight.inner_CutOff - spotlight.outer_CutOff;
	float intensity = clamp((theta - spotlight.outer_CutOff) / epsilon, 0.0, 1.0);
	vec3 ambient = spotlight.ambient * vec3(texture(material.texture_diffuse1, fs_in.TexCoord));
	vec3 diffuse = spotlight.diffuse * diff * vec3(texture(material.texture_diffuse1, fs_in.TexCoord));
	vec3 specular = spotlight.specular * spec * vec3(texture(material.texture_specular1, fs_in.TexCoord));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	diffuse *= intensity;
	specular *= intensity;
	return (ambient * 0.1 + diffuse + specular);

}
