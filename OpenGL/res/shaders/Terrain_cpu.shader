#shader vertex
#version 420 core
layout(location = 0) in vec3 position;
out float Height;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Height = position.y;
    gl_Position = projection * view * model * vec4(position.x, position.y, position.z, 1.0);
}
#shader fragment
#version 420 core
out vec4 FragColor;

in float Height;

void main()
{
    float h = (Height + 16) / 32.0f;	// shift and scale the height into a grayscale value
    FragColor = vec4(h, h, h, 1.0);
}