#shader vertex
#version 420 core
layout(location = 0) in vec3 Position;
layout(location = 5) in ivec4 boneIds;
layout(location = 6) in vec4 weights;
uniform mat4 model;
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];
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
    gl_Position = model * totalPosition;
}

#shader geometry
#version 420 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6];
out vec4 FragPos;

void main()
{
    for (int face = 0; face < 6; ++face)
    {
        gl_Layer = face;
        for (int i = 0; i < 3; ++i)
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}

#shader fragment
#version 420 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    float lightDistance = length(FragPos.xyz - lightPos);
    lightDistance = lightDistance / far_plane;//线性深度
    gl_FragDepth = lightDistance;
    //fragcolor = vec4(lightDistance, 0.0, 0.0, 0.0);
}
