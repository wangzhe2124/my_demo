#shader vertex
#version 460 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 Normal;
layout(location = 2) in vec2 TexCoord;

uniform mat4 projection;
uniform  mat4 view;
uniform mat4 model;
out VS_OUT
{
vec2 TexCoord;
}vs_out;
void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	vs_out.TexCoord = TexCoord;
};
#shader fragment
#version 460 core
out vec4 FragColor;
in VS_OUT
{
vec2 TexCoord;
}fs_in;
uniform sampler2D Texture;

uniform vec3 color;
void main()
{
	//result += vec3(texture(Texture, fs_in.TexCoord));
	FragColor = vec4(color, 1.0);
}
