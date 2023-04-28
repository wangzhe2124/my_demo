
#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 TexCoord;
out vec3 u_Color;
out vec2 u_TexCoord;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	u_Color = aColor;
	u_TexCoord = TexCoord
};
#shader fragment
#version 330 core
layout(location = 0) out vec4 color;

in vec3 u_Color;
in vec2 u_TexCoord;
uniform sampler2D u_Texture1;
uniform sampler2D u_Texture2;
uniform float AlphaValue;

void main()
{
	color = mix(texture(u_Texture1, u_TexCoord), texture(u_Texture2, u_TexCoord), AlphaValue);
};
