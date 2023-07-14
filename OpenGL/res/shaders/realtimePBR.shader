#shader vertex
#version 420 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds;
layout(location = 6) in vec4 weights;
uniform mat4 projection;
uniform  mat4 view;
uniform mat4 model;
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
void main()
{
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
    gl_Position = projection * view * model * totalPosition;
    TexCoord = aTexCoord;
    Normal = aNormal;
    FragPos = vec3(model * totalPosition);
}

#shader fragment
#version 420 core
in vec3 FragPos;

uniform vec3 direction;
uniform vec3 viewPos;
in vec2 TexCoord;
in vec3 Normal;
out vec4 color;
struct Material//材质信息
{
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    sampler2D texture_normal;
    sampler2D texture_height;
    float shininess;
};
uniform Material material;
void main()
{
    vec3 viewDir = normalize(viewPos - FragPos.xyz);
    //选择是否使用高度贴图的纹理坐标

    vec3 lightDir = normalize(direction);
    float diff = max(dot(Normal, -lightDir), 0.0);//漫反射
    //高光
    float spec;
    vec3 halfwayDir = normalize(-lightDir + viewDir);
    spec = pow(max(dot(Normal, halfwayDir), 0.0), 32);

    vec3 ambient = 5 * vec3(texture(material.texture_diffuse, TexCoord));
    vec3 diffuse = 5 * diff * vec3(texture(material.texture_diffuse, TexCoord));

    vec3 result =  (ambient * 0.1 + diffuse );       
    color = vec4(result, 1.0); //+ vec4(texture(skybox, RF).rgb, 1.0);
}
