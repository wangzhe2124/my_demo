#shader vertex
#version 460 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds;
layout(location = 6) in vec4 weights;

layout(location = 7) in mat4 instanceMatrix;
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform bool is_instance;
uniform vec2 jitter;
uniform vec2 resolution;
out VS_OUT{
 vec3 Normal;
 vec3 FragPos;
 vec2 TexCoord;
 mat3 TBN;
}vs_out;
void main()
{   
    mat4 model_matrix;
    if (is_instance)
        model_matrix = instanceMatrix;
    else
        model_matrix = model;
    //anime
    mat4 transMatrix = mat4(0.0f);
    bool no_bone = true;
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        no_bone = no_bone && (boneIds[i] == -1);
        if (boneIds[i] == -1)
        {
            
            continue;
        }
        if (boneIds[i] >= MAX_BONES)
        {
            transMatrix = mat4(1.0f);
            break;
        }
        transMatrix += finalBonesMatrices[boneIds[i]] * weights[i];          
    }
    if (no_bone)
        transMatrix = mat4(1.0f);
    vec4 totalPosition = transMatrix * vec4(Position, 1.0f);
    //jitter
    vec4 gpos = projection * view * model_matrix * totalPosition;
    mat4 jitterMat = mat4(1.0f);
    float jitterScale = 0.1 + (gpos.z / gpos.w * 0.5f + 0.5f) / 2.0f;
    jitterMat[3][0] += (jitter.x * 2 - 1.0f) * (resolution.x) * jitterScale;
    jitterMat[3][1] += (jitter.y * 2 - 1.0f) * (resolution.y) * jitterScale;

    gl_Position = jitterMat * gpos;
    vs_out.FragPos = vec3(model_matrix * totalPosition);
    vs_out.Normal = mat3(transpose(inverse(model_matrix))) * mat3(transpose(inverse(transMatrix))) * Normal;
 
    //法线变换到世界空间
    vs_out.TexCoord = TexCoord;
    vec3 T = normalize(vec3(model_matrix * transMatrix * vec4(tangent, 1.0)));
    vec3 N = normalize(vs_out.Normal);

    //T = normalize(T - dot(T, N) * N);	
    vec3 B = normalize(vec3(model_matrix * transMatrix * vec4(bitangent, 1.0)));
    //vec3 B = cross(T, N);
    vs_out.TBN = mat3(T, B, N);
};

#shader fragment
#version 460 core
layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;
layout(location = 3) out float gIndex;
in VS_OUT{
vec3 Normal;//注意顺序要与顶点着色器输出一致
vec3 FragPos;
vec2 TexCoord;
mat3 TBN;
}fs_in;

struct Material//材质信息
{
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    sampler2D texture_normal;
    sampler2D texture_height;
    float shininess;
};
uniform Material material;
uniform bool use_NormalMap;
uniform bool use_HeightMap;
uniform vec3 viewPos;
uniform float metallic;
uniform float roughness;
uniform bool isSeized;
uniform int index;
//高度（视差）贴图计算位移的纹理坐标
vec2 ParallaxMapping(vec2 TexCoord, vec3 viewDir, vec3 normal)
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
	vec2  currentTexCoords = TexCoord;
	float currentDepthMapValue = texture(material.texture_height, currentTexCoords).r;
	while (currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = texture(material.texture_height, currentTexCoords).r;
		// get depth of next layer
		currentLayerDepth += layerDepth;
	}
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
	// get depth after and before collision for linear interpolation
	float afterDepth = currentLayerDepth - currentDepthMapValue;
	float beforeDepth = texture(material.texture_height, prevTexCoords).r - (currentLayerDepth - layerDepth);

	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth + beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);
	return finalTexCoords;
}
void main()
{
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    //选择是否使用高度贴图的纹理坐标
    vec2 TexCoord = ParallaxMapping(fs_in.TexCoord, viewDir, fs_in.Normal);
    TexCoord = use_HeightMap == true ? TexCoord : fs_in.TexCoord;
    //选择是否使用法线贴图
    vec3 Normal = texture(material.texture_normal, TexCoord).rgb;
    Normal = normalize(fs_in.TBN * normalize(Normal));
    vec3 normal = use_NormalMap == true ? Normal : normalize(fs_in.Normal);

    // store the fragment position vector in the first gbuffer texture
    gPosition.xyz = fs_in.FragPos;
    gPosition.w = metallic;
    // also store the per-fragment normals into the gbuffer
    gNormal.xyz = normal;
    gNormal.w = roughness;
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(material.texture_diffuse, TexCoord).rgb;
    if (isSeized)
        gAlbedoSpec.rgb = gAlbedoSpec.rgb + vec3(0.5f);
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(material.texture_specular, TexCoord).r;
    gIndex = index;
}