#shader vertex
#version 460 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 Texcoord;

out vec2 v_texCoord;

void main(void)
{
	gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
	v_texCoord = Texcoord;
}
#shader fragment
#version 460 core

uniform sampler2D u_colorTexture;
uniform vec2 resolution;
uniform bool showEdge;
uniform bool fxaaOn;

uniform float lumaThreshold;
uniform float maxReduce;
uniform float minReduce;
uniform float maxSpan;

in vec2 v_texCoord;

out vec4 fragColor;


float Luminance(vec3 color)
{
	return dot(color, vec3(0.299, 0.587, 0.114));
}
void main(void)
{
	vec3 rgbM = texture(u_colorTexture, v_texCoord).rgb;
	// Possibility to toggle FXAA on and off.
	if (!fxaaOn)
	{
		fragColor = vec4(rgbM, 1.0);
		return;
	}
	// Sampling neighbour texels. Offsets are adapted to OpenGL texture coordinates. 
	float lumaNW = Luminance(textureOffset(u_colorTexture, v_texCoord, ivec2(-1, 1)).rgb);
	float lumaNE = Luminance(textureOffset(u_colorTexture, v_texCoord, ivec2(1, 1)).rgb);
	float lumaSW = Luminance(textureOffset(u_colorTexture, v_texCoord, ivec2(-1, -1)).rgb);
	float lumaSE = Luminance(textureOffset(u_colorTexture, v_texCoord, ivec2(1, -1)).rgb);
	float lumaM = Luminance(rgbM);

	// Gather minimum and maximum luma.
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

	// If contrast is lower than a maximum threshold ...
	if (lumaMax - lumaMin <= lumaMax * lumaThreshold)
	{
		// ... do no AA and return.
		fragColor = vec4(rgbM, 1.0);
		return;
	}

	// Sampling is done along the gradient.
	vec2 samplingDirection = normalize(vec2(-((lumaNW + lumaNE) - (lumaSW + lumaSE)), ((lumaNW + lumaSW) - (lumaNE + lumaSE))));

	// Sampling step distance depends on the luma: The brighter the sampled texels, the smaller the final sampling step direction.
	// This results, that brighter areas are less blurred/more sharper than dark areas. 
	//越亮，采样方向越短
	float samplingDirectionReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * maxReduce, minReduce);

	// 越水平或垂直，采样方向越长
	float minSamplingDirectionFactor = 1.0 / (min(abs(samplingDirection.x), abs(samplingDirection.y)) + samplingDirectionReduce);

	// Calculate final sampling direction vector by reducing, clamping to a range and finally adapting to the texture size. 
	samplingDirection = clamp(samplingDirection * minSamplingDirectionFactor, vec2(-maxSpan), vec2(maxSpan)) * resolution;

	// Inner samples on the tab.
	vec3 rgbSampleNeg = texture(u_colorTexture, v_texCoord + samplingDirection * (1.0 / 3.0 - 0.5)).rgb;
	vec3 rgbSamplePos = texture(u_colorTexture, v_texCoord + samplingDirection * (2.0 / 3.0 - 0.5)).rgb;
	vec3 rgbTwoTab = (rgbSamplePos + rgbSampleNeg) * 0.5;

	// Outer samples on the tab.
	vec3 rgbSampleNegOuter = texture(u_colorTexture, v_texCoord + samplingDirection * (0.0 / 3.0 - 0.5)).rgb;
	vec3 rgbSamplePosOuter = texture(u_colorTexture, v_texCoord + samplingDirection * (3.0 / 3.0 - 0.5)).rgb;
	vec3 rgbFourTab = (rgbSamplePosOuter + rgbSampleNegOuter) * 0.25 + rgbTwoTab * 0.5;

	// Calculate luma for checking against the minimum and maximum value.
	float lumaFourTab = Luminance(rgbFourTab);

	// Are outer samples of the tab beyond the edge ... 
	if (lumaFourTab < lumaMin || lumaFourTab > lumaMax)
	{
		// ... yes, so use only two samples.
		fragColor = vec4(rgbTwoTab, 1.0);
	}
	else
	{
		// ... no, so use four samples. 
		fragColor = vec4(rgbFourTab, 1.0);
	}
	// Show edges for debug purposes.	
	if (showEdge)
	{
		fragColor.r = 1.0;
	}
}