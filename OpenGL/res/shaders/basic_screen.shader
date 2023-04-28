#shader vertex
#version 460 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 Texcoord;

out vec2 fragTexcoord;
void main()
{
    gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
    fragTexcoord = Texcoord;
};
#shader fragment
#version 460 core
out vec4 FragColor;
in vec2 fragTexcoord;
uniform sampler2D screenTexture;

void main()
{
    FragColor = texture(screenTexture, fragTexcoord);
}
