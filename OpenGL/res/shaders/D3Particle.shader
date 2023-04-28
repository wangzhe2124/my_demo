#shader vertex
#version 460 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;
layout(location = 3) in mat4 instanceMatrix;

out vec2 fragTexcoord;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    float scale = 0.01f;
    gl_Position = projection * view * model * vec4(position * scale, 1.0f);
    fragTexcoord = TexCoord;
};
#shader fragment
#version 460 core
out vec4 FragColor;
in vec2 fragTexcoord;
uniform vec4 color;
uniform sampler2D Texture;
void main()
{
    FragColor = texture(Texture, fragTexcoord) * color;
};
