#ifndef Transform_h
#define Transform_h

#include "GLAD/glad.h"
#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"

#include <vector>

class Transform{
    
public:
    glm::vec3 Position;
    glm::mat4 Rotation;
    glm::vec3 Scale;
    glm::vec3 Right;
    glm::vec3 Front;
    Transform* Parent;
    
    Transform(const glm::vec3& position = glm::vec3(), const glm::mat4 rotation  = glm::mat4(), const glm::vec3& scale = glm::vec3())
    : Position(position), Rotation(rotation), Scale(scale), Right(glm::vec3(1,0,0)), Front(glm::vec3(0,0,-1)), Parent(nullptr)
    {}
    
    inline glm::mat4 GetModel() const
    {
        if(Parent)
        {
            glm::mat4 posMatrix = glm::translate(Position);
            glm::mat4 scaleMatrix = glm::scale(Scale);
            glm::mat4 rotationMatrix = Rotation;
            
            return Parent->GetModel() * posMatrix * scaleMatrix * rotationMatrix;
        }
        
        else
        {
            glm::mat4 posMatrix = glm::translate(Position);
            glm::mat4 scaleMatrix = glm::scale(Scale);
            glm::mat4 rotationMatrix = Rotation;
            
            return posMatrix * scaleMatrix * rotationMatrix;
        }
    }
};
#endif
