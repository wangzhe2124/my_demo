#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <sstream>
#include <vector>

#include "Debugger.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Renderer.h"
#include "KeyInput.h"
#include "Texture.h"
#include "FrameBuffer.h"

#include "ModelSpace.h"
#include "Camera.h"
//���ص�
unsigned int screenWidth = 640;
unsigned int screenHeight = 480;
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void keys_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

float deltaTime = 0.0f;	// ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��
//�������
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
//����ģ�Ϳռ�

//��������
KeyInput keyinput;

int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    //glfwWindowHint(GLFW_SAMPLES, 4);
    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Hello World", NULL, NULL);//ָ��

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

    //���ص�����
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, keys_callback);

    //ʵ����λ��
    glm::vec3 translations[100];
    int index = 0;
    float offset = 1.0f;
    for (int y = -5; y < 5; y += 1)
    {
        for (int x = -5; x < 5; x += 1)
        {
            glm::vec3 translation;
            translation.x = (float)x;
            translation.y = (float)y;
            translation.z = -3.0f;
            translations[index++] = translation;
        }
    }
    VertexBuffer instanceVb(&translations[0], sizeof(translations));
    VertexBufferLayout instanceLayout;
    instanceLayout.Push<float>(3);
    //�����������飬���㻺�壬��������
    float positions[] = {
-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,//��
-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,

-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,//ǰ
 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,//��
-0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
-0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,//��
 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

-0.5f, -0.5f, -0.5f,  0.0f,  -1.0f,  0.0f,  0.0f, 0.0f,//��
 0.5f, -0.5f, -0.5f,  0.0f,  -1.0f,  0.0f,  1.0f, 0.0f,
 0.5f, -0.5f,  0.5f,  0.0f,  -1.0f,  0.0f,  1.0f, 1.0f,
 0.5f, -0.5f,  0.5f,  0.0f,  -1.0f,  0.0f,  1.0f, 1.0f,
-0.5f, -0.5f,  0.5f,  0.0f,  -1.0f,  0.0f,  0.0f, 1.0f,
-0.5f, -0.5f, -0.5f,  0.0f,  -1.0f,  0.0f,  0.0f, 0.0f,

-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,//��
-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f
    };
    //pointlight
        //���Դλ��
    glm::vec3 pointLightPositions[] = {
    glm::vec3(2.0f,  3.0f,  0.0f),
    glm::vec3(2.0f, 3.0f, -5.0f),
    glm::vec3(-2.0f, 3.0f, -5.0f),
    glm::vec3(-2.0f,  3.0f, 0.0f)
    };

    glm::vec3 pointLightColors[] = {
     glm::vec3(1.0f, 1.0f, 1.0f),
     glm::vec3(1.0f, 1.0f, 1.0f),
     glm::vec3(1.0f, 1.0f, 1.0f),
     glm::vec3(1.0f, 1.0f, 1.0f)
    };
    VertexArray lightva;
    VertexBuffer vb(positions, sizeof(positions));
    VertexBufferLayout layout1;
    layout1.Push<float>(3);
    layout1.Push<float>(3);
    layout1.Push<float>(2);
    lightva.AddBuffer(vb, layout1);//�����㻺���붥�������
    //cube
    VertexArray cubeva;
    VertexBufferLayout layout2;
    layout2.Push<float>(3);
    layout2.Push<float>(3);
    layout2.Push<float>(2);
    cubeva.AddBuffer(vb, layout2);
    cubeva.AddBuffer(instanceVb, instanceLayout);
    glVertexAttribDivisor(3, 1);
    //��պ�
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
    //����shader
    Shader shader("res/shaders/basic.shader");
    Shader cubeshader("res/shaders/cube.shader");
    Shader skyboxShader("res/shaders/SkyBox.shader");
    Shader screenShader("res/shaders/screen.shader");
    //����Renderer
    Renderer renderer;
    //��������
    Texture texture("res/textures/container2_diffuse.png", 4);
    Texture texture2("res/textures/container2_specular.png", 4);

    //ͶӰ����
    glm::mat4 projection = glm::mat4(1.0f);

    /* Loop until the user closes the window */
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//�������

    //���ڴ���
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
    quadVa.AddBuffer(quadVb, quadLayout);//quadVa
    MSAAFrameBuffer msaa(screenWidth, screenHeight, 4);
    PostProcessing postprocessing(screenWidth, screenHeight);
    //��Ȳ���
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);//�����Ϊ1����պ�ͨ������
    //glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC1_ALPHA);
    //���޳�
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);//�޳�����
    glFrontFace(GL_CCW);//��ʱ��Ϊ����
    //���ز���
    glEnable(GL_MULTISAMPLE);
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        keyinput.ProcessMovement(window, camera, deltaTime);//���������ƶ�

        /* Render here */
        msaa.Bind();       
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        shader.Bind();
        shader.SetUniformmatri4fv("view", camera.GetViewMatrix());
        projection = glm::perspective(glm::radians(camera.Zoom), float(screenWidth / screenHeight), 0.1f, 100.0f);
        shader.SetUniformmatri4fv("projection", projection);
        for (unsigned int i = 0; i < 4; i++)
        {
            shader.SetUniform3f("material.color", pointLightColors[i]);
            ModelSpace model;
            model.Translate(pointLightPositions[i]);
            model.Scale(glm::vec3(0.5f));
            shader.SetUniformmatri4fv("model", model.GetModelSpace());
            renderer.DrawArray(lightva, shader);
        }

        cubeshader.Bind();
        cubeshader.SetUniform1i("blinn_phong", keyinput.blinn_phong);
        cubeshader.SetUniform3f("objectColor", 0.2f, 0.2f, 0.31f);
        cubeshader.SetUniform3f("viewPos", camera.Position);
        cubeshader.SetUniform1f("material.shininess", 64.0f);
        cubeshader.SetUniformmatri4fv("view", camera.GetViewMatrix());
        projection = glm::perspective(glm::radians(camera.Zoom), float(screenWidth / screenHeight), 0.1f, 100.0f);
        cubeshader.SetUniformmatri4fv("projection", projection);
        UniformBuffer viewpos(32, 0);
        //viewpos.PushUniform(camera.Position);//viewpos
        //viewpos.PushUniform(glm::vec3(0.2f, 0.2f, 0.31f));//objectcolor
        //ƽ�й�
        cubeshader.SetUniform3f("dirlight.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
        cubeshader.SetUniform3f("dirlight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        cubeshader.SetUniform3f("dirlight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        cubeshader.SetUniform3f("dirlight.direction", glm::vec3(0.0f, -5.0f, -5.0f));
        cubeshader.SetUniform1f("dirlight.LightIntensity", 20.0f); 
        //���
        for (int i = 0; i < 4; i++)
        {
            cubeshader.SetUniform3f("pointlight[" + std::to_string(i) + "].ambient", pointLightColors[i]);
            cubeshader.SetUniform3f("pointlight[" + std::to_string(i) + "].diffuse", pointLightColors[i]);
            cubeshader.SetUniform3f("pointlight[" + std::to_string(i) + "].specular", pointLightColors[i]);
            cubeshader.SetUniform3f("pointlight[" + std::to_string(i) + "].position", pointLightPositions[i]);
            cubeshader.SetUniform1f("pointlight[" + std::to_string(i) + "].LightIntensity", 10.0f);
            cubeshader.SetUniform1f("pointlight[" + std::to_string(i) + "].constant", 1.0f);
            cubeshader.SetUniform1f("pointlight[" + std::to_string(i) + "].linear", 0.14f);
            cubeshader.SetUniform1f("pointlight[" + std::to_string(i) + "].quadratic", 0.07f);
        }
        //�۹��ֵ�
        cubeshader.SetUniform3f("spotlight.ambient", 0.0f, 0.0f, 0.0f);
        cubeshader.SetUniform3f("spotlight.diffuse", 0.5f, 0.5f, 0.5f);
        cubeshader.SetUniform3f("spotlight.specular", 1.0f, 1.0f, 1.0f);
        cubeshader.SetUniform3f("spotlight.direction", camera.Front);
        cubeshader.SetUniform3f("spotlight.position", camera.Position);
        cubeshader.SetUniform1f("spotlight.constant", 1.0f);
        cubeshader.SetUniform1f("spotlight.linear", 0.009f);
        cubeshader.SetUniform1f("spotlight.quadratic", 0.0032f);
        cubeshader.SetUniform1f("spotlight.LightIntensity", 10.0f * keyinput.TorchOn);
        cubeshader.SetUniform1f("spotlight.inner_CutOff", glm::cos(glm::radians(10.0f)));//spotlight��Χ
        cubeshader.SetUniform1f("spotlight.outer_CutOff", glm::cos(glm::radians(20.0f)));//spotlight��Χ
        //���价��
        skybox.Bind(3);
        cubeshader.SetUniform1i("skybox", 3);

        ModelSpace cubemodel;
        cubeshader.SetUniformmatri4fv("model", cubemodel.GetModelSpace());
        texture.Bind(0);
        cubeshader.SetUniform1i("material.texture_diffuse1", 0);
        texture2.Bind(1);
        cubeshader.SetUniform1i("material.texture_specular1", 1);

        renderer.DrawArrayInstance(cubeva, cubeshader, 100);
        //skybox
        skyboxShader.Bind();
        glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));//��ά�Ƴ������е�λ��
        skyboxShader.SetUniformmatri4fv("view", view);
        projection = glm::perspective(glm::radians(camera.Zoom), float(screenWidth / screenHeight), 0.1f, 100.0f);
        skyboxShader.SetUniformmatri4fv("projection", projection);
        skybox.Bind();
        renderer.DrawArray(skyboxVa, skyboxShader);
        //���ز����͵����ڴ���֡����
        msaa.Read();
        postprocessing.Write();
        msaa.BlitBuffer();
        msaa.UnBind();
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        // clear all relevant buffers
        glClear(GL_COLOR_BUFFER_BIT);
        //���ڴ���
        screenShader.Bind();
        postprocessing.BindTexture();
        screenShader.SetUniform1i("gamma", keyinput.gamma);
        renderer.DrawArray(quadVa, screenShader);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);//������GLFW����ÿһ��������ɫֵ�Ĵ󻺳壨˫���壩
        /* Poll for and process events */
        glfwPollEvents();//����������������¼�
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