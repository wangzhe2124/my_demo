#shader vertex
#version 430 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 Texcoord;

out vec2 texCoord;

void main(void)
{
	gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
	texCoord = Texcoord;
}
#shader fragment
#version 430 core

uniform sampler2D colorTexture;
uniform sampler2D firstTexture;
in vec2 texCoord;
out vec4 fragColor;

uniform vec2 resolution;
uniform int searchNum;
void main(void)
{
	vec2 downDir = vec2(0, -1) * resolution;
	vec2 upDir = vec2(0, 1) * resolution;
	vec2 leftDir = vec2(-1, 0) * resolution;
	vec2 rightDir = vec2(1, 0) * resolution;
	vec4 curColor = texture(colorTexture, texCoord);
	//1
	if (texture(firstTexture, texCoord + downDir).r > 0.99)
	{
		//right
		bool rightSign;
		int rightNum;
		int i;
		for (i = 1; i < searchNum; i++)
		{
			vec2 uv = texCoord + downDir + rightDir * i;
			vec2 signal = texture(firstTexture, uv).xy;
			if (signal.r > 0.99 && signal.g < 0.01)
				continue;
			else if (signal.r < 0.01 && signal.g > 0.99)
			{
				rightSign = false;
				rightNum = i - 1;
				break;
			}
			else
			{
				rightSign = true;
				rightNum = i;
				break;
			}
		}
		if (i == searchNum)
			rightNum = i - 1;
		vec4 rightColor;
		if (rightSign)
			rightColor = texture(colorTexture, texCoord + rightNum * rightDir);
		else
			rightColor = texture(colorTexture, texCoord + downDir + rightNum * rightDir);
		//left
		bool leftSign;
		int leftNum;
		for (i = 1; i < searchNum; i++)
		{
			vec2 uv_1 = texCoord + downDir + leftDir * (i - 1);
			vec2 signal_1 = texture(firstTexture, uv_1).xy;

			vec2 uv_2 = texCoord + downDir + leftDir * i;
			vec2 signal_2 = texture(firstTexture, uv_2).xy;
			if (signal_2.r > 0.99 && signal_1.g < 0.01)
				continue;
			else if (signal_2.r < 0.01 && signal_1.g > 0.99)
			{
				leftSign = false;
				leftNum = i - 1;
				break;
			}
			else
			{
				leftSign = true;
				leftNum = i;
				break;
			}
		}
		if (i == searchNum)
			leftNum = i - 1;
		if (leftNum == searchNum && rightNum == searchNum)
		{
			fragColor = curColor;
			return;
		}
		vec4 leftColor;
		if (leftSign)
			leftColor = texture(colorTexture, texCoord + leftNum * leftDir);
		else
			leftColor = texture(colorTexture, texCoord + downDir + leftNum * leftDir);
		//mix
		if (rightSign && leftSign)
		{
			fragColor = mix(leftColor, rightColor, rightNum / (leftNum + rightNum));
		}
		else if (!rightSign && leftSign)
		{
			float ratio = (rightNum + 0.5) / (leftNum + rightNum);
			fragColor = ratio * (rightColor + leftColor) / 2 + (1 - ratio) * curColor;
		}
		else if (rightSign && !leftSign)
		{
			float ratio = (leftNum + 0.5) / (leftNum + rightNum);
			fragColor = ratio * (rightColor + leftColor) / 2 + (1 - ratio) * curColor;
		}
		else
		{
			fragColor = curColor;
		}
	}
	//2
	else if (texture(firstTexture, texCoord).g > 0.99)
	{
		//up
		bool upSign;
		int upNum;
		int i;
		for (i = 1; i < searchNum; i++)
		{
			vec2 uv_1 = texCoord + upDir * (i - 1);
			vec2 signal_1 = texture(firstTexture, uv_1).xy;

			vec2 uv_2 = texCoord + upDir * i;
			vec2 signal_2 = texture(firstTexture, uv_2).xy;

			if (signal_1.r <0.01 && signal_2.g > 0.99)
				continue;
			else if (signal_1.r < 0.01 && signal_2.g < 0.01)
			{
				upSign = false;
				upNum = i - 1;
				break;
			}
			else
			{
				upSign = true;
				upNum = i;
				break;
			}
		}
		if (i == searchNum)
			upNum = i - 1;
		vec4 upColor;
		if (upSign)
			upColor = texture(colorTexture, texCoord + upNum * upDir);
		else
			upColor = texture(colorTexture, texCoord + leftDir + upNum * upDir);
		//down
		bool downSign;
		int downNum;
		for (i = 1; i < searchNum; i++)
		{
			vec2 uv = texCoord + downDir * i;
			vec2 signal = texture(firstTexture, uv).xy;

			if (signal.r < 0.01 && signal.g > 0.99)
				continue;
			else if (signal.r < 0.01 && signal.g < 0.01)
			{
				downSign = false;
				downNum = i - 1;
				break;
			}
			else
			{
				downSign = true;
				downNum = i;
				break;
			}
		}
		if (i == searchNum)
			downNum = i - 1;
		if (downNum == searchNum && downNum == searchNum)
		{
			fragColor = curColor;
			return;
		}
		vec4 downColor;
		if (downSign)
			downColor = texture(colorTexture, texCoord + downNum * downDir);
		else
			downColor = texture(colorTexture, texCoord + leftDir + downNum * downDir);
		//mix
		if (downSign && downSign)
		{
			fragColor = mix(downColor, upColor, upNum / (upNum + downNum));
		}
		else if (!upSign && downSign)
		{
			float ratio = (upNum + 0.5) / (upNum + downNum);
			fragColor = ratio * (upColor + downColor) / 2 + (1 - ratio) * curColor;
		}
		else if (upSign && !downSign)
		{
			float ratio = (downNum + 0.5) / (downNum + upNum);
			fragColor = ratio * (upColor + downColor) / 2 + (1 - ratio) * curColor;
		}
		else
		{
			fragColor = curColor;
		}
	}
	else
		fragColor = curColor;
}