#pragma once
#ifdef _DEBUG
#pragma comment(lib, "bullet_debug.lib")
#else
#pragma comment(lib, "bullet_release.lib")
#endif // DEBUG

#include "Game.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <queue>
#include <numeric>

//回调
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void keys_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void windowSize_callback(GLFWwindow* window, int cx, int cy);

void checkMovement(GLFWwindow* window, Game& game);

void GUI_Initialize(GLFWwindow* window);

void GUI_Process(GLFWwindow* window, KeyInput& keyinput);
using namespace irrklang;

ISoundEngine* SoundEngine = createIrrKlangDevice();
int musicIndex = 0;
std::string musics[2] =
{
    "girl",
    "HUMAN"
};
unsigned int screenWidth = 960;
unsigned int screenHeight = 640;
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;	// 当前帧与上一帧的时间差
double lastFrame = 0.0f; // 上一帧的时间
Game game;
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
class Average_dt
{
private:
    std::queue<double> q;
    double sum;
    double lastTime;
public:
    Average_dt() :sum(0), lastTime(0)
    {}
    void Add(double currentTime)
    {
        double deltaTime = currentTime - lastTime;
        if (q.size() < 4)
        {
            sum += deltaTime;
            q.push(deltaTime);
        }
        else
        {
            sum += deltaTime;
            sum -= q.front();
            q.pop();
            q.push(deltaTime);
            assert(q.size() <= 4);
        }
        lastTime = currentTime;
    }
    float Average()
    {
        assert(q.size() > 0);
        return static_cast<float>(sum / q.size());
    }
}average_dt;
class TimeTest
{
private:
    float curtime;
    std::vector<std::vector<float>> times = std::vector<std::vector<float>>(5);
public:
    TimeTest()
    {
        curtime = 0;
        //times.reserve(5);
    }
    void update(bool cond, int i)
    {
        if (cond)
        {
            float t = glfwGetTime();
            times[i].push_back(t - curtime);
            curtime = t;
        }
    }
    void show(bool& cond1, bool& cond2)
    {
        if (cond1)
        {
            std::cout << "render spend:" << std::accumulate(std::begin(times[1]), std::end(times[1]), 0.0) / times[1].size() * 1000 << std::endl;
            std::cout << "swap spend:" << std::accumulate(std::begin(times[3]), std::end(times[3]), 0.0) / times[3].size() * 1000 << std::endl;
            for (int i = 0; i < times.size(); i++)
            {
                if (!times[i].empty())
                {
                    //std::cout << std::to_string(i) + ":" << std::accumulate(std::begin(times[i]), std::end(times[i]), 0.0) / times[i].size() * 1000 << std::endl;
                    times[i].clear();
                }              
            }
            cond1 = false; cond2 = false;
        }
    }
}timeTest;



int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    //glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(game.GetSwidth(), game.GetSheight(), "demo", NULL, NULL);//指针
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    //垂直同步
    glfwSwapInterval(0);
    if (glewInit() != GLEW_OK)
        std::cout << "error" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    //鼠标回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, keys_callback);
    glfwSetWindowSizeCallback(window, windowSize_callback);
    // Setup Dear ImGui context
    GUI_Initialize(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//锁定鼠标
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //blender
    //glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC1_ALPHA);
    //面剔除
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);//剔除背面
    glFrontFace(GL_CCW);//逆时针为正向
    //模板测试
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    //多重采样
    //glEnable(GL_MULTISAMPLE);

    //glEnable(GL_FRAMEBUFFER_SRGB); 
    game.ready_render();
    SoundEngine->play2D("res/music/girl.mp3", GL_TRUE);
    while (!glfwWindowShouldClose(window))
    {
        average_dt.Add(glfwGetTime());
        game.GetDeltaTime(average_dt.Average());

        checkMovement(window, game);
        //game.keyinput.ProcessMovement(window, game.camera, average_dt.Average(), game.my_state.current_energy);

        game.start_render();

        if (game.keyinput.reset_resolution)
        {
            if (game.keyinput.full_screen)
            {
                GLFWmonitor* _monitor = glfwGetPrimaryMonitor();
                const GLFWvidmode* mode = glfwGetVideoMode(_monitor);
                glfwSetWindowMonitor(window, _monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                game.SetSwidth(mode->width);
                game.SetSheight(mode->height);
            }
            else
            {
                glfwSetWindowMonitor(window, nullptr, 0, 0, 960, 640, 0);
                game.SetSwidth(960);
                game.SetSheight(640);
            }
            game.keyinput.reset_resolution = false;
        }
        //glDisable(GL_DEPTH_TEST);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //game.shaders->basicscreen_shader.Bind();
        //glBindTexture(GL_TEXTURE_2D, 0);
        ////cameradepthFBO.BindTexture();
        ////blooming_hightlightFBO.BindTexture(0,1);
        ////blooming_blur_verticalFBO.BindTexture();
        ////game.framebuffers->hdrfbo.BindTexture(0);
        ////global_dirshadowfbo.BindTexture();
        ////shadow_blur_verticalFBO.BindTexture();
        ////game.framebuffers->FXAA_FBO.BindTexture(0);
        ////game.framebuffers->gbuffer.BindTexture(0, 1);
        //game.framebuffers->csm_mapFBO[1].BindTexture();
        ////envcubemap_spec_BRDF_FBO.BindTexture();
        ////ssaoFBO.BindTexture();
        ////SpotlightMapfbo.BindTexture();
        ////glBindTexture(GL_TEXTURE_2D, Characters[10].TextureID);
        //game.renderer.DrawArray(game.vertex_arrays->quadVa, game.shaders->basicscreen_shader);
        //GUI
        GUI_Process(window, game.keyinput);
        /* Swap front and back buffers */

        glfwSwapBuffers(window);//储存着GLFW窗口每一个像素颜色值的大缓冲（双缓冲）

        /* Poll for and process events */
        glfwPollEvents();//
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
    if(!game.keyinput.ui && !game.lockEnermyMode && !game.keyinput.dialogMode)
        game.camera.ProcessMouseMovement(xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mode)
{
    if (!game.Get_Response_action() && !game.keyinput.ui && !game.keyinput.dialogMode)
    {
        game.processMouseInuput(button, action, mode);
    }
    if (game.keyinput.dialogMode)
    {
        bool quit = game.processUserDialogInuput(button, action, mode, lastX, lastY);
        if (quit)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//锁定鼠标
        }
    }
}

void keys_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    //quit
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    {
        game.lockEnermyMode = !game.lockEnermyMode;
        if (game.lockEnermyMode)
        {
            game.updateLockedModel();
        }
    }
    //ui
    if (key == GLFW_KEY_U && action == GLFW_PRESS)
    {
        game.keyinput.ui = !game.keyinput.ui;
        if (game.keyinput.ui)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);//解锁鼠标
            firstMouse = true;
        }
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//锁定鼠标
    }
    //dialog
    if (key == GLFW_KEY_B && action == GLFW_PRESS)
    {
        game.keyinput.dialogMode = !game.keyinput.dialogMode;
        if (game.keyinput.dialogMode)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);//解锁鼠标
            firstMouse = true;
        }
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//锁定鼠标
    }
    //printScreen
    if (key == GLFW_KEY_PRINT_SCREEN && action == GLFW_PRESS)
    {
        game.saveImage();
    }
    //changeMusic
    if (key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS)
    {
        musicIndex++;
        if (musicIndex > 1)
            musicIndex = 0;
        SoundEngine->stopAllSounds();
        std::cout << musicIndex << std::endl;
        SoundEngine->play2D(("res/music/" + musics[musicIndex] + ".mp3").c_str(), GL_TRUE);
    }
    game.processKeyInuput(key, action, mode);
    //mode's value: shift(1), ctrl(2), alt(4), win(8), and multi is the add;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    game.camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


void windowSize_callback(GLFWwindow* window, int cx, int cy)
{
    std::cout << "resolution:" << cx << "," << cy << std::endl;
    game.ResetResolution(cx, cy);
}
void GUI_Process(GLFWwindow* window, KeyInput& keyinput)
{
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    //GUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
          // Edit bools storing our window open/close state
    ImGui::Checkbox("Gamma", &keyinput.gamma);
    ImGui::Checkbox("shake screen", &keyinput.shake_screen);
    ImGui::SliderInt("shake frames", &keyinput.shake_frames, 1, 100);
    ImGui::SliderInt("shake level", &keyinput.shake_level, 1, 50);
    ImGui::Checkbox("cutoff query/octree", &keyinput.cutoff_query);
    ImGui::Checkbox("pickMode", &keyinput.pickMode);

    ImGui::Checkbox("use terrain", &keyinput.use_terrain);
    ImGui::Checkbox("HeightMap", &keyinput.useheight);
    ImGui::Checkbox("NormalMap", &keyinput.NormalMap);
    ImGui::Checkbox("useEnvLight", &keyinput.EnvLight_spec);
    ImGui::Checkbox("usePBR / blinPhong", &keyinput.usePBR);
    ImGui::Checkbox("third_view", &keyinput.third_view);
    ImGui::Checkbox("free_view", &keyinput.free_view);
    ImGui::Checkbox("blur shadow", &keyinput.blur_shadow);
    ImGui::Checkbox("show mesh", &keyinput.show_mesh);
    ImGui::Checkbox("fullscreen", &keyinput.full_screen);
    ImGui::SameLine();
    ImGui::Checkbox("reset resolution", &keyinput.reset_resolution);
    ImGui::SliderFloat("camera speed", &game.camera.MovementSpeed, 0.0f, 1000.0f);

    ImGui::SliderFloat("Metallic", &keyinput.metallic, 0.0f, 1.0f);
    ImGui::SliderFloat("Roughness", &keyinput.roughness, 0.0f, 1.0f);
    
    
    ImGui::SliderFloat("exposure", &keyinput.exposure, 0.0f, 100.0f);
    ImGui::SliderInt("tess level", &keyinput.tess_level, 0, 3);
    //sun
    ImGui::Checkbox("sun Window", &keyinput.sun_window);
    if (keyinput.sun_window)
    {
        ImGui::Begin("sun Window", &keyinput.sun_window);
        if (ImGui::Button("Close Me"))
            keyinput.sun_window = false;
        ImGui::SliderFloat("SunIntensity", &keyinput.SunIntensity, 0.0f, 10.0f, "%5.f", 1.0f);
        ImGui::Separator();
        ImGui::SliderFloat("sm_bias", &keyinput.sun_sm_bias, 0.0f, 1.0f);
        ImGui::SliderFloat("sun speed", &keyinput.sun_speed, 0.0f, 50.0f);
        ImGui::SliderFloat("sun pcf radius", &keyinput.sun_pcf_radius, 0.0f, 10.0f);
        ImGui::Checkbox("sun pcf", &keyinput.sun_pcf);

        ImGui::ColorEdit3("sun color", (float*)&keyinput.SunColor); // Edit 3 floats representing a color
        ImGui::End();
    }
    ImGui::SameLine();
    //pointlight
    ImGui::Checkbox("pointlight Window", &keyinput.pointlight_window);
    if (keyinput.pointlight_window)
    {
        ImGui::Begin("pointlight Window", &keyinput.pointlight_window);
        if (ImGui::Button("Close Me"))
            keyinput.pointlight_window = false;
        ImGui::SliderFloat("pt_sm_radius", &keyinput.point_sm_radius, 0.0f, 10.0f);
        ImGui::SliderFloat("pt_intensity", &keyinput.pointlight_Intensity, 0.0f, 10.0f);
        ImGui::SliderFloat("pt_far_plane", &keyinput.point_far_plane, 0.0f, 20.0f);
        ImGui::Checkbox("pt_sm_pcf", &keyinput.point_sm_pcf);
        ImGui::ColorEdit3("pt_color", (float*)&keyinput.point_color);
        ImGui::End();
    }
    ImGui::SameLine();
    //spotlight
    ImGui::Checkbox("spotlight Window", &keyinput.spotlight_window);
    if (keyinput.spotlight_window)
    {
        ImGui::Begin("spot Window", &keyinput.spotlight_window);
        if (ImGui::Button("Close Me"))
            keyinput.spotlight_window = false;
        ImGui::Checkbox("Torch", &keyinput.TorchOn);
        ImGui::SliderFloat("bias_x", &keyinput.st_bias_x, -5.0f, 5.0f);
        ImGui::SliderFloat("bias_y", &keyinput.st_bias_y, -5.0f, 5.0f);
        ImGui::SliderFloat("bias_z", &keyinput.st_bias_z, 0.0f, 5.0f);
        ImGui::SliderFloat("st_far_plane", &keyinput.spot_far_plane, 5.0f, 50.0f);
        ImGui::SliderFloat("torch intensity", &keyinput.torch_intensity, 0.0f, 50.0f);
        ImGui::ColorEdit3("torch color", (float*)&keyinput.torch_color);

        ImGui::End();
    }
    //bloom
    ImGui::Checkbox("bloom Window", &keyinput.bloom_window);
    if (keyinput.bloom_window)
    {
        ImGui::Begin("bloom Window", &keyinput.bloom_window);
        if (ImGui::Button("Close Me"))
            keyinput.bloom_window = false;
        ImGui::SliderInt("bloom times", &keyinput.bloom_times, 0, 16);
        ImGui::SliderFloat("bloom halox", &keyinput.bloom_halox, 0.0f, 1.5f);
        ImGui::SliderFloat("bloom haloy", &keyinput.bloom_haloy, 0.0f, 1.5f);
        ImGui::SliderFloat("bloom haloz", &keyinput.bloom_haloz, 0.0f, 1.5f);
        ImGui::SliderInt("bloom radius", &keyinput.bloom_radius, 0, 16);
        ImGui::SliderFloat("bloom edge", &keyinput.bloom_edge, 0.1f, 5.0f);
        ImGui::End();
    }
    ImGui::Checkbox("assist screen", &keyinput.assist_screen);
    //ssao
    ImGui::Checkbox("SSAO Window", &keyinput.SSAO_window);
    if (keyinput.SSAO_window)
    {
        ImGui::Begin("SSAO Window", &keyinput.SSAO_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        if (ImGui::Button("Close Me"))
            keyinput.SSAO_window = false;
        ImGui::Checkbox("SSAO", &keyinput.useSSAO);
        ImGui::SliderFloat("SSAO sample radius", &keyinput.SSAO_radius, 0.0f, 10.0f);
        ImGui::SliderFloat("SSAO sample bias", &keyinput.SSAO_bias, 0.0f, 20.0f);
        ImGui::SliderFloat("SSAO sample rangecheck", &keyinput.SSAO_rangecheck, 0.0f, 1.0f);
        ImGui::End();
    }
    ImGui::SameLine();
    //particle
    ImGui::Checkbox("particle Window", &keyinput.particle_window);
    if (keyinput.particle_window)
    {
        ImGui::Begin("particle Window", &keyinput.particle_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        if (ImGui::Button("Close Me"))
            keyinput.particle_window = false;
        ImGui::Checkbox("2d particle", &keyinput.show_particle);
        ImGui::SliderFloat("particle scale", &keyinput.particle_scale, 1.0f, 100.0f);
        ImGui::SliderFloat("particle offset", &keyinput.particle_offset, 0.0f, 15.0f);
        ImGui::SliderInt("new particle num", &keyinput.new_particle_num, 1, 20);
        ImGui::SliderFloat("particle velocity", &keyinput.particle_vel, 0.0f, 10.0f);
        ImGui::SliderFloat("particle life reduce", &keyinput.particle_life_reduce, 0.0f, 0.05f);
        ImGui::Checkbox("3d particle", &keyinput.show_d3particle);
        ImGui::End();
    }
    ImGui::SameLine();

    //animation
    ImGui::Checkbox("animation Window", &keyinput.animation_window);
    if (keyinput.animation_window)
    {
        ImGui::Begin("animation Window", &keyinput.animation_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        if (ImGui::Button("Close Me"))
            keyinput.animation_window = false;
        ImGui::Checkbox("change animation", &keyinput.chage_animation);
        ImGui::SliderInt("animation type", &keyinput.animation_type, 0, Animations::MAX);
        ImGui::End();
    }
    //fxaa
    ImGui::Checkbox("FXAA Window", &keyinput.fxaa_window);
    if (keyinput.fxaa_window)
    {
        ImGui::Begin("FXAA Window", &keyinput.fxaa_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        if (ImGui::Button("Close Me"))
            keyinput.fxaa_window = false;
        ImGui::Checkbox("fxaa on", &keyinput.fxaa_on);
        ImGui::Checkbox("fxaa showEdge", &keyinput.fxaa_showEdge);
        ImGui::SliderFloat("fxaa lumaThreshold", &keyinput.fxaa_lumaThreshold, 0.0f, 1.0f);
        ImGui::SliderFloat("fxaa mulReduce", &keyinput.fxaa_maxReduce, 0.0f, 1.0f);
        ImGui::SliderFloat("fxaa minReduce", &keyinput.fxaa_minReduce, 0.0f, 1.0f);
        ImGui::SliderFloat("fxaa maxSpan", &keyinput.fxaa_maxSpan, 0.0f, 32.0f);
        ImGui::End();
    }
    //TAA
    ImGui::Checkbox("TAA Window", &keyinput.taa_window);
    if (keyinput.taa_window)
    {
        ImGui::Begin("TAA Window", &keyinput.taa_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        if (ImGui::Button("Close Me"))
            keyinput.taa_window = false;
        ImGui::Checkbox("taa on", &keyinput.taa_on);
        ImGui::SliderFloat("taa diffScale", &keyinput.taa_mixWeight, 0.0f, 1.0f);
        ImGui::End();
    }
    //MLAA
    ImGui::Checkbox("MLAA Window", &keyinput.mlaa_window);
    if (keyinput.mlaa_window)
    {
        ImGui::Begin("MLAA Window", &keyinput.mlaa_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        if (ImGui::Button("Close Me"))
            keyinput.mlaa_window = false;
        ImGui::Checkbox("mlaa on", &keyinput.mlaa_on);
        ImGui::SliderFloat("mlaa threShold", &keyinput.mlaa_threShold, 0.0f, 1.0f);
        ImGui::SliderInt("mlaa searchNum", &keyinput.mlaa_searchNum, 0, 96);
        ImGui::End();
    }
    static int counter = 0;
    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    if (keyinput.ui)
    {
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    ImGui::EndFrame();
}

void GUI_Initialize(GLFWwindow* window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 130";
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void checkMovement(GLFWwindow* window, Game& game)
{
    if (!game.Get_Response_action())
    {
        //game.keyinput.ProcessMovement(window, game.camera, deltaTime, game.my_state.current_energy);//键盘输入移动
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                game.updatePositions(FORWARD_LEFT);
            }
            else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                game.updatePositions(FORWARD_RIGHT);
            }
            else
            {
                game.updatePositions(FORWARD);
            }
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                game.updatePositions(BACKWARD_LEFT);
            }
            else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                game.updatePositions(BACKWARD_RIGHT);

            }
            else
            {
                game.updatePositions(BACKWARD);

            }
        }
        else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            game.updatePositions(LEFT);

        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            game.updatePositions(RIGHT);

        }
        else
            game.updatePositions(STOP);

        {
            float& energy = game.my_state.current_energy;
            bool& dashing = game.my_state.dashing;
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            {

                if (game.my_state.current_energy > 0)
                {
                    game.my_state.dashing = true;
                    energy -= 0.05f;
                    if (energy < 0)
                        energy = 0.0f;
                }
                else
                    dashing = false;

            }
            else
            {
                dashing = false;
                energy += 1.0f;
                if (energy > 100.0f)
                    energy = 100.0f;
            }
        }
    }
    else
        game.updatePositions(STOP);
}