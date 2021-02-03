#ifndef CAMERA_H
#define CAMERA_H

#include "GLAD/glad.h"
#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"

#include <vector>

#include "Transform.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
};

enum Movement_Status {
    CAMERA_ONLY,
    PLAYER_ONLY,
    START
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  0.001f;
const float SENSITIVITY =  0.1f;

extern Movement_Status movement;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    
    Transform *child;
    

    // constructor with vectors
    Camera(glm::vec3 position, Transform& transform, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        child = &transform;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
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
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, Movement_Status movement, float deltaTime)
    {
        
        if(movement == CAMERA_ONLY)
            MovementSpeed = 1.0f;
        else
            MovementSpeed = 1.0f;
        
        float velocity = MovementSpeed * deltaTime;
        
        if (direction == FORWARD)
        {
            if(movement == CAMERA_ONLY)
            {
                Position += Front * velocity;
            }
            
            else if(movement == PLAYER_ONLY)
            {
                child->Position += child->Front * velocity;
            }
            
        }
        if (direction == BACKWARD)
        {
            if(movement == CAMERA_ONLY)
            {
                Position -= Front * velocity;
            }
                      
            else if(movement == PLAYER_ONLY)
            {
                child->Position -= child->Front * velocity;
            }
                      
        }
        if (direction == LEFT)
        {
           if(movement == CAMERA_ONLY)
           {
               Position -= Right * velocity;
           }
           
           else if(movement == PLAYER_ONLY)
           {
               child->Position -= child->Right * velocity;
           }
           
        }
        if (direction == RIGHT)
        {
            if(movement == CAMERA_ONLY)
            {
                Position += Right * velocity;
            }
            
            else if(movement == PLAYER_ONLY)
            {
                child->Position += child->Right * velocity;
            }
        }
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
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
#endif
