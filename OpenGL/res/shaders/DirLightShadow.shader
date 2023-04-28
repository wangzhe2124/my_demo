#shader vertex
#version 460 core
layout(location = 0) in vec3 Position;		
layout(location = 5) in ivec4 boneIds;
layout(location = 6) in vec4 weights;
uniform mat4 LightSpace;
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
    gl_Position = LightSpace * model * totalPosition;
	//gl_Position = LightSpace * model * vec4(Position, 1.0f);
};
#shader fragment
#version 460 core
//out vec4 fragcolor;
void main()
{
	gl_FragDepth = gl_FragCoord.z;
	//fragcolor = vec4(gl_FragCoord.z, 0.0, 0.0, 1.0);
}
