#pragma once
#include "KeyInput.h"
#include <iostream>

KeyInput::KeyInput()
    :dialogMode(false), pickMode(false),
    shake_screen(false), shake_frames(45), shake_level(5),
    gamma(true),
    exposure(2.0f),
    useSSAO(true),
    ui(false),
    NormalMap(false),
    useheight(false),
    metallic(0.5f),
    roughness(0.8f),
    use_terrain(false),
    usePBR(true),
    //resolution
    full_screen(false), reset_resolution(false),
    blur_shadow(false),
    EnvLight_spec(true),
    //ssao
    SSAO_window(false), SSAO_bias(8.0f), SSAO_radius(0.25f), SSAO_rangecheck(0.446f),
    assist_screen(false),
    tess_level(1),

    third_view(true), free_view(false), show_mesh(false),
    //
    animation_window(false), chage_animation(false), animation_type(0),
    //bloom
    bloom_window(false), bloom_times(8), bloom_halox(0.8f), bloom_haloy(0.8f), bloom_haloz(0.8f), bloom_edge(2.0f), bloom_radius(1),
    //sun
    sun_window(false), SunIntensity(3.0f), SunColor(1.0f), sun_sm_bias(0.4f), sun_speed(0.0f), sun_pcf(true), sun_pcf_radius(1.0f),
    //spotlight
    spotlight_window(false), st_bias_x(0.15f), st_bias_y(0.15f), st_bias_z(0.15f), spot_far_plane(20.0f), TorchOn(false), torch_color(1.0f), torch_intensity(5.0f),
    //pointlight
    pointlight_window(false), point_sm_radius(2.0f), point_sm_pcf(false), pointlight_Intensity(0.0f), point_far_plane(5.0f), point_color(1.0f),
    //particles
    particle_window(false), show_particle(false), particle_scale(30.0f), particle_offset(1.0f), new_particle_num(1), particle_vel(1.0f), particle_life_reduce(0.037f), show_d3particle(false),
    //fxaa
    fxaa_window(false), fxaa_on(false), fxaa_showEdge(false), fxaa_lumaThreshold(0.2f), fxaa_maxReduce(0.001f), fxaa_minReduce(0.001f), fxaa_maxSpan(8.0f),
    //taa
    taa_window(false), taa_on(true), taa_mixWeight(0.3f),
    //mlaa
    mlaa_window(false), mlaa_on(true), mlaa_threShold(0.5f), mlaa_searchNum(8),
    show_times(false), sample_times(false),
    //cutoff
    cutoff_query(false)
{

}
void KeyInput::ProcessMovement(GLFWwindow* window, Camera& camera, float deltaTime, float& energy)
{

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            camera.ProcessKeyboard(FORWARD_LEFT, deltaTime);
            camera.character_Front = FORWARD_LEFT;
            camera.is_move = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            camera.ProcessKeyboard(FORWARD_RIGHT, deltaTime);
            camera.character_Front = FORWARD_RIGHT;
            camera.is_move = true;
        }
        else
        {
            camera.ProcessKeyboard(FORWARD, deltaTime);
            camera.character_Front = FORWARD;
            camera.is_move = true;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            camera.ProcessKeyboard(BACKWARD_LEFT, deltaTime);
            camera.character_Front = BACKWARD_LEFT;
            camera.is_move = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            camera.ProcessKeyboard(BACKWARD_RIGHT, deltaTime);
            camera.character_Front = BACKWARD_RIGHT;
            camera.is_move = true;
        }
        else
        {
            camera.ProcessKeyboard(BACKWARD, deltaTime);
            camera.character_Front = BACKWARD;
            camera.is_move = true;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
        camera.character_Front = LEFT;
        camera.is_move = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
        camera.character_Front = RIGHT;
        camera.is_move = true;
    }
    else
        camera.is_move = false;
}
void KeyInput::RecordKey(int key, int action)
{
    if (key >= 0 && key <= 1024)
    {
        if (action == GLFW_PRESS && keysReleased[key])
        {
            keysPressed[key] = true;
            keysReleased[key] = false;
        }
        else if (action == GLFW_RELEASE)
        {
            keysPressed[key] = false;
            keysReleased[key] = true;
        }
    }

    float time = static_cast<float>(glfwGetTime());
    Chord_Key::key_element k = { key, time };
    if (sequence_k.empty())
    {
        sequence_k.push(k);
    }
    else
    {
        if (sequence_k.size() > 3)
            sequence_k.pop();
        while (!sequence_k.empty() && (time - sequence_k.front().time) > 1.0f)
        {
            sequence_k.pop();
        }
        sequence_k.push(k);
        //processSequence();
    }
}
void KeyInput::ProcessKey(int key, int action)
{


    //按T开关手电筒
    if (key == GLFW_KEY_T && action == GLFW_PRESS)
    {
        TorchOn = !TorchOn;
        std::cout << "torch : " << (TorchOn ? "on" : "off") << std::endl;
    }

    //按G切换gamma校正
    if (key == GLFW_KEY_G && action == GLFW_PRESS)
    {
        gamma = !gamma;
        std::cout << "gamma校正 : " << (gamma ? "on" : "off") << std::endl;
    }

    if (key == GLFW_KEY_H && action == GLFW_PRESS)
    {
        useheight = !useheight;
        std::cout << "heightmap : " << (useheight ? "on" : "off") << std::endl;
    }
    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        useSSAO = !useSSAO;
        std::cout << "SSAO : " << (useSSAO ? "on" : "off") << std::endl;
    }
    if (key == GLFW_KEY_N && action == GLFW_PRESS)
    {
        NormalMap = !NormalMap;
        std::cout << "NormalMap : " << (NormalMap ? "on" : "off") << std::endl;
    }

}
