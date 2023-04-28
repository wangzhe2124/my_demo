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
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;
in vec2 fragTexcoord;
uniform sampler2D screenTexture;
uniform float edge;
void main()
{
    //反相
    //FragColor = vec4(vec3(1.0 - texture(screenTexture, fragTexcoord)), 1.0); 

    vec3 color = texture(screenTexture, fragTexcoord).rgb ;
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));//提取高亮
    if (brightness > edge)
        BrightColor = vec4(color.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    //float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    //FragColor = vec4(average, average, average, 1.0);
    //vec3 col = Convolution();//卷积
    FragColor = vec4(color, 1.0);

}
