#shader vertex
#version 430 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 Texcoord;

out vec2 v_texCoord;

void main(void)
{
	gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
	v_texCoord = Texcoord;
}
#shader fragment
#version 430 core

uniform sampler2D u_colorTexture;

in vec2 v_texCoord;
out vec2 fragColor;
uniform float threShold;
float Luminance(vec3 color)
{
	return dot(color, vec3(0.299, 0.587, 0.114));
}
void main(void)
{
	vec3 rgbM = texture(u_colorTexture, v_texCoord).rgb;

	float lumaN = Luminance(textureOffset(u_colorTexture, v_texCoord, ivec2(0, 1)).rgb);
	float lumaW = Luminance(textureOffset(u_colorTexture, v_texCoord, ivec2(-1, 0)).rgb);

	float lumaM = Luminance(rgbM);
	//up

	if (abs(lumaM - lumaN) > max(lumaM, lumaN) * threShold)
		fragColor.r = 1;
	else
		fragColor.r = 0;
	//left
	if (abs(lumaM - lumaW) > max(lumaM, lumaW) * threShold)
		fragColor.g = 1;
	else
		fragColor.g = 0;
}