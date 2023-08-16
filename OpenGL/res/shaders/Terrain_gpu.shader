#shader vertex
#version 450 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
out vec2 TexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
    TexCoord = uv;
    gl_Position = vec4(position, 1.0);
}

#shader tess_cont
#version 450 core
layout(vertices = 4) out;

uniform mat4 model;
uniform mat4 view;
uniform int tess_level;
in vec2 TexCoord[];
out vec2 TextureCoord[];

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];

    if (gl_InvocationID == 0)
    {
        const int MIN_TESS_LEVEL = 1;
        const int MAX_TESS_LEVEL = 4;
        const float MIN_DISTANCE = 10;
        const float MAX_DISTANCE = 1000;

        vec4 eyeSpacePos00 = view * model * gl_in[0].gl_Position;
        vec4 eyeSpacePos01 = view * model * gl_in[1].gl_Position;
        vec4 eyeSpacePos10 = view * model * gl_in[2].gl_Position;
        vec4 eyeSpacePos11 = view * model * gl_in[3].gl_Position;

        // "distance" from camera scaled between 0 and 1
        float distance00 = clamp((abs(eyeSpacePos00.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
        float distance01 = clamp((abs(eyeSpacePos01.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
        float distance10 = clamp((abs(eyeSpacePos10.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
        float distance11 = clamp((abs(eyeSpacePos11.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);

        float tessLevel0 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance10, distance00));
        float tessLevel1 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance00, distance01));
        float tessLevel2 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance01, distance11));
        float tessLevel3 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance11, distance10));

        gl_TessLevelOuter[0] = tessLevel0 * tess_level;
        gl_TessLevelOuter[1] = tessLevel1 * tess_level;
        gl_TessLevelOuter[2] = tessLevel2 * tess_level;
        gl_TessLevelOuter[3] = tessLevel3 * tess_level;

        gl_TessLevelInner[0] = tessLevel1 * tess_level;
        gl_TessLevelInner[1] = tessLevel2 * tess_level;
    }
}


#shader tess_eval
#version 450 core
layout(quads, fractional_odd_spacing, cw) in;

uniform sampler2D heightMap;
uniform sampler2D textureMap;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec2 TextureCoord[];

out float Height;
out vec3 color;
out vec3 FragPos;
vec4 interpolate(in vec4 v0, in vec4 v1, in vec4 v2, in vec4 v3)
{
    vec4 a = mix(v0, v1, gl_TessCoord.x);
    vec4 b = mix(v2, v3, gl_TessCoord.x);
    return mix(a, b, gl_TessCoord.y);
}
void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec2 t00 = TextureCoord[0];
    vec2 t01 = TextureCoord[1];
    vec2 t10 = TextureCoord[2];
    vec2 t11 = TextureCoord[3];

    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
    vec2 texCoord = (t1 - t0) * v + t0;

    Height = texture(heightMap, texCoord).x * 64.0 - 16.0;
    color = texture(textureMap, texCoord).xyz;

    vec4 p = interpolate(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position)
        + vec4(0, Height, 0, 0);
    //p.y += Height;
    gl_Position = projection * view * model * p;
    FragPos = vec3(model * p);
}

#shader fragment
#version 450 core
layout(location = 0) out vec3 Position;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec4 FragColor;
in float Height;
in vec3 color;

in vec3 FragPos;
void main()
{
    Position = FragPos;
    vec3 x = dFdx(FragPos); // "FragPos = vec3(model * p);" from the tess eval
    vec3 y = dFdy(FragPos);
    Normal = normalize(cross(x, y));
    float h = (Height + 16) / 64.0f;	// shift and scale the height into a grayscale value
    FragColor = vec4(h, h, h, 1.0) * vec4(color, 1.0);
}