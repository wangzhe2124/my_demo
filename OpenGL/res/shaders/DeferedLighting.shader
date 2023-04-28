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
layout(location = 0) out vec4 color;
#define SPLITNUM 4
#define POINT_LIGHTS_NUM 4
const float PI = 3.14159265359;

struct DirLight//平行光
{
	vec3 position;
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
uniform vec3 objectColor;


in VS_OUT{
vec2 TexCoord;
}fs_in;

vec3 CalcDirLight(vec3 normal, vec2 Texcoord, vec3 FragPos, float occlusion);
vec3 CalcPointLight(int i, vec3 normal, vec2 Texcoord, vec3 FragPos, float occlusion);
vec3 CalcSpotLight(vec3 normal, vec2 Texcoord, vec3 FragPos, float occlusion);
vec3 CalcEnvLight(vec3 normal, vec2 Texcoord, vec3 FragPos, float occlusion);
//DGF
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0, float roughness);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform samplerCube EnvLight;
uniform samplerCube EnvLight_spec;
uniform sampler2D EnvLight_spec_BRDF;

uniform sampler2D predirshadow;
uniform sampler2D prepointshadow;
uniform sampler2D prespotshadow;

uniform mat4 globallightSpaceMatrix;
uniform sampler2D ssao;
uniform bool useSSAO;
uniform bool useEnvLight;
uniform bool usePBR;
float metallic;
float roughness;
void main()
{
	vec2 Texcoord = fs_in.TexCoord;
	vec4 gPos = texture(gPosition, Texcoord);
	vec3 FragPos = gPos.rgb;
	metallic = gPos.a;
	vec4 gNorm = texture(gNormal, Texcoord);
	vec3 normal = gNorm.rgb;
	roughness = gNorm.a;
	float occlusion = useSSAO == true ? texture(ssao, Texcoord).r : 1.0;
	vec3 result = CalcDirLight(normal, Texcoord, FragPos, occlusion);
	for (int i = 0; i < POINT_LIGHTS_NUM; i++)
	{
		result += CalcPointLight(i, normal, Texcoord, FragPos, occlusion);
	}
	result += CalcSpotLight(normal, Texcoord, FragPos, occlusion);
	if(useEnvLight)
		result += CalcEnvLight(normal, Texcoord, FragPos, occlusion);

	color = vec4(result, 1.0); //+ vec4(texture(skybox, RF).rgb, 1.0);
};
vec3 CalcDirLight(vec3 normal, vec2 Texcoord, vec3 FragPos, float occlusion)
{
	vec4 texColor = texture(gAlbedoSpec, Texcoord);
	vec3 Albedo = texColor.rgb;
	float Specular = texColor.a;
	vec3 viewDir = normalize(camera.viewPos - FragPos);
	vec3 lightDir = normalize(dirlight.direction);
	vec3 halfwayDir = normalize(-lightDir + viewDir);
	float shadow = texture(predirshadow, Texcoord).r;
	if (usePBR)
	{
		//DFG	
		vec3 Lo = vec3(0.0);
		vec3 F0 = vec3(0.04);
		F0 = mix(F0, Albedo, metallic);
		//float distance = length(dirlight.position - FragPos);
		float attenuation = 1.0;// / (distance * distance);
		vec3 radiance = dirlight.color * attenuation * max(dot(normal, -lightDir), 0.0);
		float NDF = DistributionGGX(normal, halfwayDir, roughness);
		float G = GeometrySmith(normal, viewDir, -lightDir, roughness);
		vec3 F = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0, roughness);
		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;
		vec3 specular = (NDF * G * F) / (4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, -lightDir), 0.0) + 0.001);
		Lo = (kD * Albedo / PI + specular) * radiance;
		vec3 ambient = vec3(0.03) * Albedo * occlusion;
		vec3 color = ambient + Lo * (1 - shadow);
		color = color / (vec3(1.0f) + color);
		return color * dirlight.LightIntensity;
	}
	else
	{
		float diff = max(dot(normal, -lightDir), 0.0);//漫反射
		//高光
		float spec = pow(max(dot(normal, halfwayDir), 0.0), 16);
		return (Albedo * (0.1 + diff) + Specular * spec) * dirlight.LightIntensity * (1 - shadow);
	}
}

vec3 CalcPointLight(int i, vec3 normal, vec2 Texcoord, vec3 FragPos, float occlusion)
{
	PointLight pt = pointlight[i];
	vec3 Albedo = texture(gAlbedoSpec, Texcoord).rgb;
	vec3 viewDir = normalize(camera.viewPos - FragPos);
	vec3 lightDir = normalize(FragPos - pt.position);
	float shadow = 0;
	vec4 tex_shadow = texture(prepointshadow, Texcoord).rgba;
	if (i == 0)
		shadow = tex_shadow.r;
	else if(i  ==1)
		shadow = tex_shadow.g;
	else if(i == 2)
		shadow = tex_shadow.b;
	else
		shadow = tex_shadow.a;
	vec3 color;
	//DFG	
	float distance = length(pt.position - FragPos) + 0.001f;
	if (distance < pt.far_plane)
	{
		vec3 Lo = vec3(0.0);
		vec3 F0 = vec3(0.04);
		F0 = mix(F0, Albedo, metallic);
		vec3 halfwayDir = normalize(-lightDir + viewDir);
		
		float attenuation = pow(max(1.0f - pow(distance / pt.far_plane, 2), 0), 2);
		vec3 radiance = pt.color * max(dot(normal, -lightDir), 0.0);
		float NDF = DistributionGGX(normal, halfwayDir, roughness);
		float G = GeometrySmith(normal, viewDir, -lightDir, roughness);
		vec3 F = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0, roughness);
		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;
		vec3 specular = (NDF * G * F) / (4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, -lightDir), 0.0) + 0.001);
		Lo = (kD * Albedo / PI + specular) * radiance;
		vec3 ambient = vec3(0.03) * Albedo * occlusion;
		color = (ambient + Lo * (1 - shadow)) * attenuation;
		color = color / (vec3(1.0f) + color);
	}
	else
		color = vec3(0);
	return color * pt.LightIntensity;
}

vec3 CalcSpotLight(vec3 normal, vec2 Texcoord, vec3 FragPos, float occlusion)
{
	vec3 Albedo = texture(gAlbedoSpec, Texcoord).rgb;
	vec3 viewDir = normalize(camera.viewPos - FragPos);
	vec3 lightDir = normalize(FragPos - spotlight.position);
	float shadow = texture(predirshadow, Texcoord).g;
	//DFG	
	vec3 Lo = vec3(0.0);
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, Albedo, metallic);
	vec3 halfwayDir = normalize(-lightDir + viewDir);
	float distance = length(spotlight.position - FragPos) + 0.001f;
	float attenuation = pow(max(1.0f - pow(distance / spotlight.far_plane, 2), 0), 2);
	vec3 radiance = spotlight.color * max(dot(normal, -lightDir), 0.0);
	float NDF = DistributionGGX(normal, halfwayDir, roughness);
	float G = GeometrySmith(normal, viewDir, -lightDir, roughness);
	vec3 F = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0, roughness);
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;
	vec3 specular = (NDF * G * F) / (4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, -lightDir), 0.0) + 0.001);
	Lo = (kD * Albedo / PI + specular) * radiance;
	vec3 ambient = vec3(0.03) * Albedo * occlusion;
	float theta = dot(lightDir, normalize(spotlight.direction));
	float epsilon = spotlight.inner_CutOff - spotlight.outer_CutOff;
	float intensity = pow(clamp((theta - spotlight.outer_CutOff) / epsilon, 0.0, 1.0),2);
	vec3 color;
	if (distance < spotlight.far_plane)
	{
		color = (ambient+ Lo * (1 - shadow)) * intensity * attenuation;
		color = color / (vec3(1.0f) + color);
	}
	else
		color = vec3(0);
	return color * spotlight.LightIntensity;
}

//DGF
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;
	float denom = pow((NdotH2 * (a2 - 1.0) + 1.0), 2);

	return a2 / (PI *denom);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;
	return NdotV / (NdotV * (1.0 - k) + k);
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
vec3 CalcEnvLight(vec3 normal, vec2 Texcoord, vec3 FragPos, float occlusion)
{
	vec3 Albedo = pow(texture(gAlbedoSpec, Texcoord).rgb, vec3(1/2.2));
	vec3 viewDir = normalize(camera.viewPos - FragPos);
	//vec3 lightDir = normalize(FragPos - spotlight.position);
	//vec3 halfwayDir = normalize(-lightDir + viewDir);
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, Albedo, metallic);
	vec3 F = fresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, roughness);
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	vec3 irradiance = texture(EnvLight, normal).rgb;
	vec3 diffuse = irradiance * Albedo;
	//spec
	vec3 R = reflect(-viewDir, normal);
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefilteredColor = textureLod(EnvLight_spec, R, roughness * MAX_REFLECTION_LOD).rgb;

	vec2 envBRDF = texture(EnvLight_spec_BRDF, vec2(max(dot(normal, viewDir), 0.0), roughness)).rg;
	vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

	vec3 ambient = (kD * diffuse + specular) * occlusion;

	vec3 color = ambient / (vec3(1.0) + ambient);
	return color;
}