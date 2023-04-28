#shader vertex
#version 450 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 Texcoord;

out VS_OUT{
 vec2 TexCoord;
}vs_out;
void main(void)
{
	gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
	vs_out.TexCoord = Texcoord;
}

#shader fragment
#version 450 core
out vec4 fragColor;
uniform sampler2D preFrameColor;
uniform sampler2D currentFrameColor;
uniform sampler2D alias_texture;
uniform vec2 resolution;
in VS_OUT{
vec2 TexCoord;
}fs_in;

uniform float mixWeight;

void main(void)
{

	vec4 curColor = texture2D(currentFrameColor, fs_in.TexCoord);
	vec4 preColor = texture2D(preFrameColor, fs_in.TexCoord);
	//up
	vec2 signCur = texture2D(alias_texture, fs_in.TexCoord).xy;
	float signUp = signCur.r;
	float signLeft = signCur.g;
	//float signDown = texture2D(alias_texture, fs_in.TexCoord + vec2(0, -1) * resolution).r;
	//float signRight = texture2D(alias_texture, fs_in.TexCoord + vec2(1, 0) * resolution).g;
	if(signUp > 0.99 || signLeft > 0.99 )
		fragColor = mix(curColor, preColor, mixWeight * 2);
	else
		fragColor = mix(curColor, preColor, mixWeight);
}