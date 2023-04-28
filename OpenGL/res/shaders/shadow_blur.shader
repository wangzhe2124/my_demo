#shader vertex
#version 420 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 Texcoord;

out vec2 fragTexcoord;
void main()
{
    gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
    fragTexcoord = Texcoord;

};

#shader fragment
#version 420 core
layout(location = 0) out vec4 FragColor;
in vec2 fragTexcoord;
uniform sampler2D image;

uniform bool horizontal;
//uniform float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
uniform float weight[5] = float[](0.3, 0.15, 0.07, 0.05, 0.03);

void main()
{
    vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
    vec3 result = texture(image, fragTexcoord).rgb * weight[0]; // current fragment's contribution
    if (horizontal)
    {
        for (int i = 1; i < 5; ++i)
        {
            result += texture(image, fragTexcoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, fragTexcoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for (int i = 1; i < 5; ++i)
        {
            result += texture(image, fragTexcoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(image, fragTexcoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}