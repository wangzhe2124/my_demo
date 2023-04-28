#shader vertex
#version 420 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;

out VS_OUT{
 vec2 TexCoord;
}vs_out;
void main()
{
    gl_Position = vec4(Position, 1.0f);
    vs_out.TexCoord = TexCoord;
};

#shader fragment
#version 420 core
layout(location = 0) out float color;
in VS_OUT{
 vec2 TexCoord;
}fs_in;
uniform sampler2D ssaoInput;

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, fs_in.TexCoord + offset).r;
        }
    }
    color = result / (4.0 * 4.0);
}