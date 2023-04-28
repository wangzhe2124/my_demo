#include <GL/glew.h>
#include <GLFW/glfw3.h>


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
//鼠标回调
unsigned int screenWidth = 960;
unsigned int screenHeight = 640;
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void keys_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

float deltaTime = 0.0f;	// 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
//创建相机
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
//键盘输入
KeyInput keyinput;

int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_SAMPLES, 4);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Hello World", NULL, NULL);//指针

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);
    if (glewInit() != GLEW_OK)
        std::cout << "error" << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;

    //鼠标回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, keys_callback);
    //点光源属性
    float pointlights[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,//前
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,//后
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,//左
    -0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

     0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,//右
     0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,//下
     0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,//上
    -0.5f,  0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f
    };
    VertexArray pointlightVa;
    VertexBuffer pointlightVb(pointlights, sizeof(pointlights));
    VertexBufferLayout pointlightLayout;
    pointlightLayout.Push<float>(3);
    pointlightLayout.Push<float>(3);
    pointlightLayout.Push<float>(2);
    pointlightVa.AddBuffer(pointlightVb, pointlightLayout);
    //点光源属性
    glm::vec3 pointLightPositions[] = {
    glm::vec3(2.0f,  2.0f,  2.0f),
    glm::vec3(2.0f, 2.0f, -2.0f),
    glm::vec3(-2.0f, 2.0f, -2.0f),
    glm::vec3(-2.0f,  2.0f, 2.0f)
    };
    glm::vec3 pointLightColors[] = {
     glm::vec3(0.8f, 0.8f, 0.6f),
     glm::vec3(0.8f, 0.8f, 0.6f),
     glm::vec3(0.8f, 0.8f, 0.6f),
     glm::vec3(0.8f, 0.8f, 0.6f)
    };

    //shaders
    Shader screenShader("res/shaders/screen.shader");
    Shader pointlightshader("res/shaders/basic.shader");
    Shader modelshader("res/shaders/model.shader");
    D3Shader model_geometry_shader("res/shaders/model_geomrtry.shader");
    Shader model_instance_shader("res/shaders/model_instance.shader");
    Shader tencil("res/shaders/single.shader");
    Shader skyboxShader("res/shaders/SkyBox.shader");

    //天空盒
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    VertexArray skyboxVa;
    VertexBuffer skyboxVb(skyboxVertices, sizeof(skyboxVertices));
    VertexBufferLayout skyboxLayout;
    skyboxLayout.Push<float>(3);
    skyboxVa.AddBuffer(skyboxVb, skyboxLayout);
    std::vector<std::string> faces
    {
        "res/textures/skybox/right.jpg",
        "res/textures/skybox/left.jpg",
        "res/textures/skybox/top.jpg",
        "res/textures/skybox/bottom.jpg",
        "res/textures/skybox/front.jpg",
        "res/textures/skybox/back.jpg"
    };
    CubeMapTexture skybox(faces);
    //创建Renderer
    Renderer renderer;
     //投影矩阵
    glm::mat4 projection = glm::mat4(1.0f);
    //models
    Model ourModel("res/objects/nanosuit_upgrade/nanosuit.obj");
    Model Marry("res/objects/Marry/Marry.obj");
    Model Planet("res/objects/planet/planet.obj");
    Model Rock("res/objects/rock/rock.obj");
    Model sakura("res/objects/sakura/sakura.obj");
    //小行星位置
    unsigned int amount = 10000;
    std::vector<glm::mat4> modelMatrices(amount);
    std::srand(glfwGetTime()); // 初始化随机种子    
    float radius = 75.0;
    float offset = 15.0f;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        // 1. 位移：分布在半径为 'radius' 的圆形上，偏移的范围是 [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // 让行星带的高度比x和z的宽度要小
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. 缩放：在 0.05 和 0.25f 之间缩放
        float scale = (rand() % 20) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));

        // 3. 旋转：绕着一个（半）随机选择的旋转轴向量进行随机的旋转
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. 添加到矩阵的数组中
        modelMatrices[i] = model;
    }

    VertexBuffer RockVb(&modelMatrices[0], modelMatrices.size() * sizeof(glm::mat4));
    for (unsigned int i = 0; i < Rock.meshes.size(); i++)
    {
        unsigned int VAO = Rock.meshes[i].VAO;
        glBindVertexArray(VAO);
        // 顶点属性
        GLsizei vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
        glBindVertexArray(0);
    }
    //后期处理 + 多重采样
        //quadVa后期处理
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    VertexArray quadVa;
    VertexBuffer quadVb(quadVertices, sizeof(quadVertices));
    VertexBufferLayout quadLayout;
    quadLayout.Push<float>(2);
    quadLayout.Push<float>(2);
    quadVa.AddBuffer(quadVb, quadLayout);
    MSAAFrameBuffer msaa(screenWidth, screenHeight, 4);
    PostProcessing postprocessing(screenWidth, screenHeight);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//锁定鼠标
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //blender
    //glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC1_ALPHA);
    //面剔除
    //glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);//剔除背面
    glFrontFace(GL_CCW);//逆时针为正向
    //模板测试
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    //多重采样
    glEnable(GL_MULTISAMPLE);
    /* Loop until the user closes the window */
    //gamma校正
    //glEnable(GL_FRAMEBUFFER_SRGB);
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        keyinput.ProcessMovement(window, camera, deltaTime);//键盘输入移动
        /* Render here */
        msaa.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);//深度，模板测试
        glEnable(GL_DEPTH_TEST);
        //点光源
        pointlightshader.Bind();
        pointlightshader.SetUniformmatri4fv("view", camera.GetViewMatrix());
        projection = glm::perspective(glm::radians(camera.Zoom), float(screenWidth / screenHeight), 0.1f, 1000.0f);
        pointlightshader.SetUniformmatri4fv("projection", projection);
        for (unsigned int i = 0; i < 4; i++)
        {
            pointlightshader.SetUniform3f("material.color", pointLightColors[i]);
            ModelSpace model;
            model.Translate(pointLightPositions[i]);
            model.Scale(glm::vec3(0.5f));
            pointlightshader.SetUniformmatri4fv("model", model.GetModelSpace());
            renderer.DrawArray(pointlightVa, pointlightshader);
        }


        //实例化小行星
        model_instance_shader.Bind();
        model_instance_shader.SetUniformmatri4fv("view", camera.GetViewMatrix());
        projection = glm::perspective(glm::radians(camera.Zoom), float(screenWidth / screenHeight), 0.1f, 1000.0f);
        model_instance_shader.SetUniformmatri4fv("projection", projection);

        model_instance_shader.SetUniform3f("objectColor", 0.2f, 0.2f, 0.2f);
        model_instance_shader.SetUniform1f("material.shininess", 64.0f);
        model_instance_shader.SetUniform3f("viewPos", camera.Position);
        //平行光

        model_instance_shader.SetUniform3f("dirlight.ambient", 0.05f, 0.05f, 0.1f);
        model_instance_shader.SetUniform3f("dirlight.diffuse", 0.2f, 0.2f, 0.7f);
        model_instance_shader.SetUniform3f("dirlight.specular", 0.7f, 0.7f, 0.7f);
        model_instance_shader.SetUniform3f("dirlight.direction", -0.2f, -1.0f, -0.3f);
        model_instance_shader.SetUniform1f("dirlight.LightIntensity", 1.0f);
        //点光
        for (int i = 0; i < 4; i++)
        {
            model_instance_shader.SetUniform3f("pointlight[" + std::to_string(i) + "].ambient", pointLightColors[i]);
            model_instance_shader.SetUniform3f("pointlight[" + std::to_string(i) + "].diffuse", pointLightColors[i]);
            model_instance_shader.SetUniform3f("pointlight[" + std::to_string(i) + "].specular", pointLightColors[i]);
            model_instance_shader.SetUniform3f("pointlight[" + std::to_string(i) + "].position", pointLightPositions[i]);
            model_instance_shader.SetUniform1f("pointlight[" + std::to_string(i) + "].LightIntensity", 10.0f);
            model_instance_shader.SetUniform1f("pointlight[" + std::to_string(i) + "].constant", 1.0f);
            model_instance_shader.SetUniform1f("pointlight[" + std::to_string(i) + "].linear", 0.14f);
            model_instance_shader.SetUniform1f("pointlight[" + std::to_string(i) + "].quadratic", 0.07f);
        }
        //聚光手电
        model_instance_shader.SetUniform3f("spotlight.ambient", 0.0f, 0.0f, 0.0f);
        model_instance_shader.SetUniform3f("spotlight.diffuse", 1.0f, 1.0f, 1.0f);
        model_instance_shader.SetUniform3f("spotlight.specular", 1.0f, 1.0f, 1.0f);
        model_instance_shader.SetUniform3f("spotlight.direction", camera.Front);
        model_instance_shader.SetUniform3f("spotlight.position", camera.Position);
        model_instance_shader.SetUniform1f("spotlight.constant", 1.0f);
        model_instance_shader.SetUniform1f("spotlight.linear", 0.009f);
        model_instance_shader.SetUniform1f("spotlight.quadratic", 0.0032f);

        model_instance_shader.SetUniform1f("spotlight.LightIntensity", 1.0f);
        model_instance_shader.SetUniform1f("spotlight.inner_CutOff", glm::cos(glm::radians(10.5f)));//spotlight范围
        model_instance_shader.SetUniform1f("spotlight.outer_CutOff", glm::cos(glm::radians(12.5f)));//spotlight范围
        for (unsigned int i = 0; i < Rock.meshes.size(); i++)
        {
            glBindVertexArray(Rock.meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, Rock.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
        }

        //天空盒
        skyboxShader.Bind();
        glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));//降维移除第四列的位移
        skyboxShader.SetUniformmatri4fv("view", view);
        projection = glm::perspective(glm::radians(camera.Zoom), float(screenWidth / screenHeight), 0.1f, 100.0f);
        skyboxShader.SetUniformmatri4fv("projection", projection);
        skybox.Bind();
        renderer.DrawArray(skyboxVa, skyboxShader);
        //后期处理
        msaa.Read();
        postprocessing.Write();
        msaa.BlitBuffer();
        msaa.UnBind();
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        // clear all relevant buffers
        glClear(GL_COLOR_BUFFER_BIT);
        screenShader.Bind();
        screenShader.SetUniform1i("gamma", keyinput.gamma);
        postprocessing.BindTexture();
        renderer.DrawArray(quadVa, screenShader);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);//储存着GLFW窗口每一个像素颜色值的大缓冲（双缓冲）

        /* Poll for and process events */
        glfwPollEvents();//触发键盘鼠标输入事件
    }
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
bool keysPressed[1024];
bool keysReleased[1024] = { true };
void keys_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
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
    keyinput.ProcessKey(window, key, action);
}