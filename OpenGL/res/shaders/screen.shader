#shader vertex
#version 420 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 Texcoord;

out vec2 fragTexcoord;
void main()
{
	gl_Position =vec4(position.x, position.y, 0.0f, 1.0f);
	fragTexcoord = Texcoord;
};
#shader fragment
#version 420 core
out vec4 FragColor;
in vec2 fragTexcoord;
uniform sampler2D screenTexture;
uniform sampler2D assistTexture;
uniform sampler2D blooming_screenTexture;
const float offset = 1.0 / 300.0;
uniform bool gamma;
uniform float exposure;
vec3 Convolution();
void main()
{
	//反相
	//FragColor = vec4(vec3(1.0 - texture(screenTexture, fragTexcoord)), 1.0); 
	//灰度图
    vec3 HDRcolor = texture(screenTexture, fragTexcoord).rgb;
    HDRcolor += texture(blooming_screenTexture, fragTexcoord).rgb;
    HDRcolor = vec3(1.0) - exp(-HDRcolor * exposure);//HDRcolor / (HDRcolor + vec3(1.0));
    vec3 Color;
    if (gamma)
    {
        FragColor = vec4(pow(HDRcolor, vec3(1 / 2.4)) * 1.055f - vec3(0.055f), 1.0);
    }
    else
    {
        FragColor = vec4(HDRcolor, 1.0);
    }
    //if (gl_FragCoord.x > 600 && gl_FragCoord.y > 400)
    //{
    //    FragColor = vec4(texture(assistTexture, fragTexcoord).rgb,1.0);
    //}

	//float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
	//FragColor = vec4(average, average, average, 1.0);
    //vec3 col = Convolution();//卷积
    //FragColor = vec4(col, 1.0);

}

vec3 Convolution()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset, offset), // 左上
        vec2(0.0f, offset), // 正上
        vec2(offset, offset), // 右上
        vec2(-offset, 0.0f),   // 左
        vec2(0.0f, 0.0f),   // 中
        vec2(offset, 0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2(0.0f, -offset), // 正下
        vec2(offset, -offset)  // 右下
        );

    float kernel[9] = float[](
        1.0 / 16, 1.0 / 16, 1.0 / 16,
        1.0 / 16, 8.0 / 16, 1.0 / 16,
        1.0 / 16, 1.0 / 16, 1.0 / 16
        );

    vec3 sampleTex[9];
    for (int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, fragTexcoord.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for (int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    return col;
}