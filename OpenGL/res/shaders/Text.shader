#shader vertex
#version 420 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 Texcoord;

out vec2 fragTexcoord;
uniform mat4 projection;
void main()
{
    gl_Position = projection * vec4(position.x, position.y, 0.0f, 1.0f);
    fragTexcoord = Texcoord;
};
#shader fragment
#version 420 core
out vec4 FragColor;
in vec2 fragTexcoord;
uniform sampler2D text;
uniform vec3 textColor;
void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, fragTexcoord).r);
    FragColor = vec4(textColor, 1.0) * sampled;
}
