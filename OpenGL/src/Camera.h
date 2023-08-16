#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    FORWARD_LEFT,
    FORWARD_RIGHT,
    BACKWARD_LEFT,
    BACKWARD_RIGHT,
    STOP
};

// Default camera values
const float YAW = 0.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;//Î»ÖÃ
    bool third_view;
    bool free_view;
    bool is_move;
    unsigned int character_Front;
    glm::vec3 Front;//·½Ïò
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    float near_plane;
    float far_plane;
    unsigned int screen_width;
    unsigned int screen_height;
    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),float np = 0.1f, float fp = 20000.0f, unsigned int sw = 960, unsigned int sh = 640,
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) 
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), character_Front(FORWARD),is_move(false)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        near_plane = np;
        far_plane = fp;
        screen_width = sw;
        screen_height = sh;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), character_Front(FORWARD),is_move(false)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        if (third_view)
        {
            return glm::lookAt(Get_third_position(), Position, Up);
        }
        else
            return glm::lookAt(Get_first_position(), Position + Front, Up);
    }
    glm::vec3 Get_third_position()
    {
        return Position - glm::vec3(3.0f) * Front;
    }
    glm::vec3 Get_first_position()
    {
        return Position + glm::vec3(0.4f) * Front;
    }
    void Set_third_view(bool t)
    {
        third_view = t;
    }
    void Set_free_view(bool t)
    {
        free_view = t;
    }
    glm::mat4 GetProjectionMatrix()
    {
        return glm::perspective(glm::radians(Zoom), float(screen_width / screen_height), near_plane, far_plane);
    }
    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(int direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        float y_value = free_view == true ? Front.y : 0.0f;
        if (direction == 0)
            Position += glm::normalize(glm::vec3(Front.x, y_value ,Front.z)) * velocity;
        if (direction == 1)                             
            Position -= glm::normalize(glm::vec3(Front.x, y_value , Front.z)) * velocity;
        if (direction == 2)                              
            Position -= glm::normalize(glm::vec3(Right.x, y_value , Right.z)) * velocity;
        if (direction == 3)                              
            Position += glm::normalize(glm::vec3(Right.x, y_value , Right.z)) * velocity;
        if (direction == 4)
            Position += (Front - Right) * velocity * glm::vec3(0.707f);
        if (direction == 5)
            Position += (Front + Right) * velocity * glm::vec3(0.707f);
        if (direction == 6)
            Position += (-Front - Right) * velocity * glm::vec3(0.707f);
        if (direction == 7)
            Position += (-Front + Right) * velocity * glm::vec3(0.707f);
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -60.0f)
                Pitch = -60.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front = glm::vec3(1.0f);
        front.x = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = -cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
