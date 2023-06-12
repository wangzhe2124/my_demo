#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <random>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <functional>
#include <set>
#include <irrKlang.h>
#include "Debugger.h"
#include "Shader.h"
#include "KeyInput.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Model.h"
#include "ModelSpace.h"
#include "Camera.h"
#include "Renderer.h"
#include "FrameBuffer.h"
#include "CSM.h"
#include "vender/imgui/imgui.h"
#include "vender/imgui/imgui_impl_glfw.h"
#include "vender/imgui/imgui_impl_opengl3.h"
#include "Config.h"
#include <ft2build.h>
#include "Text.h"
#include FT_FREETYPE_H 
#include "Particle.h"
#include "Animator.h"
#include "ScenGraph.h"
#include "Tools.h"
#include "bullet.h"

constexpr auto CSM_SPLIT_NUM = 4;
constexpr auto POINT_LIGHTS_NUM = 4;
constexpr auto PI = 3.14159265359;
struct Sun_DATA
{
    glm::vec3 Sun_Position;
    glm::vec3 Sun_Direction;
    Sun_DATA():Sun_Position(0.0f),Sun_Direction(0.0f)
    {};
    Sun_DATA(glm::vec3 sp, glm::vec3 sd) :Sun_Position(sp), Sun_Direction(sd)
    {};
};


struct SpotLight_DATA
{
    float near_plane = 0.1f;
    float far_plane = 20.0f;
    glm::vec3 bias_direction;
    glm::vec3 position;
    glm::vec3 direction;
    glm::mat4 SpotlightSpaceMatrix;
};

struct PointLight_DATA
{
    float lightintensity;
    float near_plane;
    float far_plane;
    glm::vec3 position[POINT_LIGHTS_NUM];
    ModelSpace space[POINT_LIGHTS_NUM];
    glm::vec3 color[POINT_LIGHTS_NUM];
    PointLight_DATA() : lightintensity(2.0f), near_plane(0.1f), far_plane(25.0f),position(),space(), color()
    {};
};
struct Character_state
{
    float max_life;
    float current_life;
    float max_magic;
    float current_magic;
    float max_energy;
    float current_energy;
    bool dashing;
    int movingState;
    int movingAction;
    float rotation;
    Character_state() :max_life(200.0f), current_life(150.0f) ,
        max_magic(100.0f), current_magic(100.0f),
        max_energy(100.0f), current_energy(100.0f), dashing(false), movingState(0), movingAction(0), rotation(0)
    {}
};
class Game
{
private:
    unsigned int screenWidth = 960;
    unsigned int screenHeight = 640;
    TerrainData terrain;
    float deltaTime;
    Animations* animations;
    bool response_action;
    Entity::Octree octree = Entity::Octree(10);
    std::set<animeModel*> inFrustumModels;
    std::map<float, animeModel*> sortedModels;
    int frameIndex;
    Bt bt;
    int seizeIndex;
public:
    Character_state my_state;
    KeyInput keyinput;
    Camera camera = Camera(glm::vec3(0.0f, 1.4f, 3.0f));
    CSM_Dirlight csm_dirlight = CSM_Dirlight(camera, CSM_SPLIT_NUM);
    Shaders* shaders;
    Textures* textures;
    Models* models;
    Pre_FrameBuffers* pre_framebuffers;
    FrameBuffers* framebuffers;
    VertexArrays* vertex_arrays;
    VertexBuffers* vertex_buffers;
    Renderer renderer;
    Sun_DATA sun;
    SpotLight_DATA spotlight;
    PointLight_DATA pointlight;
    bool Last_CSM_update = true;
    bool lockEnermyMode;
    int shakeFrames;
    myTools::dialogBox bbox = myTools::dialogBox(screenWidth * 0.4f, screenWidth * 0.6f, screenHeight * 0.8f, screenHeight * 0.9f, "example");
    animeModel* lockedEnermy = nullptr;
    glm::mat4 Last_CSM_update_matrix;
    Particle_Generator particle_generator = Particle_Generator(500, 1, 0.002f);
    D3Particle_Generator d3particle_generator = D3Particle_Generator(500, 1, 0.002f);
    std::string strConfigFileName = std::string("src/config.ini");
    std::fstream out = std::fstream(strConfigFileName);
    Config config = Config(strConfigFileName);
    Game() :shaders(nullptr), textures(nullptr), models(nullptr), framebuffers(nullptr), pre_framebuffers(nullptr), vertex_arrays(nullptr), vertex_buffers(nullptr) {};
    ~Game() {
        delete shaders;
        delete textures;
        delete models;
        delete framebuffers;
        delete pre_framebuffers;
        delete vertex_arrays;
        delete vertex_buffers;
        delete animations;
    }


    inline void read_config()
    {
        out.open(strConfigFileName, std::ios::app);
        if (out.is_open()) {
            //¶ÁÈ¡config.ini
            out >> config;
            out.close();
        }
    }
    template <typename T1, typename T2, typename T3>
    void Gaussian_Blured_Texture(int j, unsigned int times, Shader& blooming_blurshader, T1& PreShadowFBO, T2& shadow_blur_horizontalFBO,
        T3& shadow_blur_verticalFBO, Renderer& renderer, VertexArray& quadVa);
    inline void GetDeltaTime(float t)
    {
        deltaTime = t;
    }
    void ready_render();

    void start_render();
    void drawBillboard(glm::vec2 scale, glm::vec2 offset, float alpha);
    void Generate_TextParticle();

    void processMouseInuput(int key, int action, int mode);
    bool processUserDialogInuput(int key, int action, int mode, float cursorX, float cursorY);
    void shakeScreen(int i);
    void processKeyInuput(int key, int action, int mode);
    void updateLockedModel()
    {
        std::map<float, animeModel*>::iterator it = sortedModels.begin();
        for (std::map<float, animeModel*>::iterator it = sortedModels.begin(); it != sortedModels.end(); it++)
        {
            std::vector<float> aabb = it->second->aabb;
            glm::vec3 center = glm::vec3((aabb[min_x] + aabb[max_x]) * 0.5f, (aabb[min_y] + aabb[max_y]) * 0.5f, (aabb[min_z] + aabb[max_z]) * 0.5f);
            glm::vec3 cameraPos = camera.Get_first_position();
            glm::vec3 viewDir = glm::normalize(center - cameraPos);
            if (glm::dot(viewDir, camera.Front) > 0.85f)
            {
                lockedEnermy = it->second;
                break;
            }
        }
        if (it == sortedModels.end())
        {
            lockedEnermy = nullptr;
        }
    }
    bool isInFrustum(std::vector<float>& aabb);
    bool isInFrustum(glm::vec3& pointMin, glm::vec3& pointMax);
    void cutOffFrustum();
    void drawModels(Shader& shader);
    void cutOffTree(Entity::Octree& o);
    void drawModelsShadow(Shader& shader);
    void drawModelsAABB();
    void initializeBullet();
    glm::vec3 collisionUseBullet();
    void renderBtWorld(Shader& shader);
    void updatePositions(int dir);
    void saveImage();
    void ProcessAction();
    inline unsigned int GetSwidth() { return screenWidth; }
    inline void SetSwidth(unsigned int sw) { screenWidth = sw; }
    inline unsigned int GetSheight() { return screenHeight; }
    inline void SetSheight(unsigned int sh) { screenHeight = sh; }
    inline bool Get_Response_action() { return response_action; }
    inline void set_Response_action(bool b) { response_action = b; }
    void Update_Sun();

    void Update_Spotlight();

    void Initialize_Pointlight();
    void Initialize_Vertex_Arrays();
    void Initialize_Models_Positions();
    void Initialize_Terrain_gpu();
    void Update_Pointlight();

    void Generate_Dir_CSM();

    void Generate_Point_SM();

    void Generate_Spot_SM();

    void Generate_Defered_basicDATA();

    void Generate_SSAO();

    void Generate_SSAO_blur();

    void Generate_PreShadow();

    void Generate_Origin_Screen();

    void Generate_SkyBox();

    void Generate_PostProcess();

    void Generate_CubeTexture();

    void Generate_EnvLight_Diffuse();

    void Generate_EnvLight_Specular();

    void Generate_EnvLight_Specular_BRDF();

    void Generate_Terrain_gpu();

    void Update_Models_Positions();


    void RenderText(Shader& text_shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

    void Debug();

    void Generate_Particle();
    void Generate_D3Particle();
    void Generate_Health_bar();
    void Generate_Health_bar_enemy();
    std::string Collision_detection();
    inline void ResetResolution(unsigned int width, unsigned int height)
    {
        framebuffers->~FrameBuffers();
        framebuffers = new FrameBuffers(width, height);
    }
};
