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
uniform sampler2D equirectangularMap;
const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invAtan;
	uv += 0.5;
	return uv;
}

void main()
{
	vec2 uv = SampleSphericalMap(normalize(fs_in.pos)); // make sure to normalize localPos
	vec3 color = texture(equirectangularMap, uv).rgb;
	FragColor = vec4(color, 1.0);
}

