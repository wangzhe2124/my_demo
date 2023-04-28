#shader vertex
#version 460 core
layout(location = 0) in vec3 position;

uniform mat4 projection;
uniform  mat4 view;
out VS_OUT
{
vec3 pos;
}vs_out;
void main()
{
	gl_Position = projection * view * vec4(position, 1.0f);
	vs_out.pos = position;
};
#shader fragment
#version 460 core
out vec4 FragColor;
in VS_OUT
{
vec3 pos;
}fs_in;
uniform samplerCube environmentMap;
const float PI = 3.14159265359;
void main()
{
    vec3 normal = normalize(fs_in.pos);
    vec3 irradiance = vec3(0.0);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, normal);
    up = cross(normal, right);

    float sampleDelta = 0.025;
    float nrSamples = 0.0;
    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

            irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    FragColor = vec4(irradiance, 1.0);
}

