#ifndef __RECTANGLE__
#define __RECTANGLE__

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <iostream>
#include "Shader.h"

class Rectangle
{
    float scaleX, scaleY;
    float rotation;
    glm::vec3 translation;
    glm::vec4 color;
    glm::mat4 model;

public:
    Rectangle(
        float scaleX = 1.0f,
        float scaleY = 1.0f,
        float rotation = 0.0f,
        glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
    );
    
    void draw(Shader& shader, unsigned int VAO, unsigned int VBO, unsigned int EBO);

    void setScale(float scaleX, float scaleY);
    void setRotation(float rotation);
    void setTranslation(glm::vec3 translation);
    void setColor(glm::vec4 newColor);

    glm::vec3 getPosition() const;
    glm::vec4 getColor() const;
};

#endif
