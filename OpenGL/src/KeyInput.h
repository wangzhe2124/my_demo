#pragma once
#include "Camera.h"
#include <GLFW/glfw3.h>
#include <queue>
#include <unordered_map>
#include <string>
namespace Chord_Key
{
    struct key_element
    {
        int key;
        float time;
    };
}

class KeyInput
{
private:
    std::queue<Chord_Key::key_element> sequence_k;
    std::unordered_map<std::string, int> control_map;
public:
    bool keysPressed[1024];
    bool keysReleased[1024] = { true };
    bool dialogMode;
    bool pickMode;
    bool shake_screen;    int shake_frames; int shake_level;
    bool gamma;
    float exposure;
    float metallic;
    float roughness;
    bool use_terrain;
    bool useheight;
    bool NormalMap;
    bool useSSAO;
    bool ui;
    //resolution
    bool full_screen; bool reset_resolution;
    bool blur_shadow;
    bool EnvLight_spec;
    bool usePBR;
    float SSAO_bias;
    float SSAO_radius;
    float SSAO_rangecheck;
    bool SSAO_window;
    bool assist_screen;
    int tess_level;
    bool third_view; bool free_view; bool show_mesh;
    //animation
    bool animation_window; bool chage_animation; int animation_type;
    //bloom
    bool bloom_window; int bloom_times; float bloom_halox; float bloom_haloy; float bloom_haloz; float bloom_edge; int bloom_radius;
    //sun
    bool sun_window; float SunIntensity; glm::vec3 SunColor; float sun_sm_bias; float sun_speed; bool sun_pcf; float sun_pcf_radius;
    //spotlight
    bool spotlight_window; float st_bias_x; float st_bias_y; float st_bias_z; float spot_far_plane; bool TorchOn; glm::vec3 torch_color; float torch_intensity;
    //pointlight
    bool pointlight_window; float point_sm_radius; bool point_sm_pcf; float pointlight_Intensity; float point_far_plane; glm::vec3 point_color;
    //particle
    bool particle_window; bool show_particle; float particle_scale; float particle_offset; int new_particle_num; float particle_vel; float particle_life_reduce; bool show_d3particle;
    //fxaa
    bool fxaa_window; bool fxaa_on; bool fxaa_showEdge; float fxaa_lumaThreshold; float fxaa_maxReduce; float fxaa_minReduce; float fxaa_maxSpan;
    //TAA
    bool taa_window; bool taa_on; float taa_mixWeight;
    //MLAA
    bool mlaa_window; bool mlaa_on; float mlaa_threShold; int mlaa_searchNum;
    //
    bool show_times; bool sample_times;
    //
    bool cutoff_query;
    KeyInput();
    void RecordKey(int key, int action);
    void ProcessKey(int key, int action);
    void ProcessMovement(GLFWwindow *window, Camera& camera, float deltaTime, float& energy);
};


