#shader vertex
#version 450 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 Texcoord;

out vec2 fragTexcoord;
void main()
{
    gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
    fragTexcoord = Texcoord;
};
#shader fragment
#version 450 core
out vec4 FragColor;
in vec2 fragTexcoord;
uniform float screenHeight;

uniform float max_life;
uniform float current_life;
uniform float max_magic;
uniform float current_magic;
uniform float max_energy;
uniform float current_energy;
float life_height_down = screenHeight - 40;
float life_height_up = screenHeight - 30;

float magic_height_down = screenHeight - 60;
float magic_height_up = screenHeight - 50;

float energy_height_down = screenHeight - 80;
float energy_height_up = screenHeight - 70;

float x_left = 20.0f;
void main()
{
    bool is_life= gl_FragCoord.y > life_height_down && gl_FragCoord.y < life_height_up;
    bool is_magic = gl_FragCoord.y > magic_height_down && gl_FragCoord.y < magic_height_up;
    bool is_energy = gl_FragCoord.y > energy_height_down && gl_FragCoord.y < energy_height_up;
    //life
    if (gl_FragCoord.x > x_left && gl_FragCoord.x < (max_life + x_left) && is_life)
    {
        if (gl_FragCoord.x < (current_life + x_left))
            FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        else
            FragColor = vec4(vec3(0), 1.0);
    }
    //magic
    else if (gl_FragCoord.x > x_left && gl_FragCoord.x < (max_magic + x_left) && is_magic)
    {
        if (gl_FragCoord.x < (current_magic + x_left))
            FragColor = vec4(0.0, 0.0, 1.0, 1.0);
        else
            FragColor = vec4(vec3(0), 1.0);
    }
    //energy
    else if (gl_FragCoord.x > x_left && gl_FragCoord.x < (max_energy + x_left) && is_energy)
    {
        if (gl_FragCoord.x < (current_energy + x_left))
            FragColor = vec4(0.0, 1.0, 0.0, 1.0);
        else
            FragColor = vec4(vec3(0), 1.0);
    }
    else
        discard;
}
