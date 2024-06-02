#ifndef __CIRCLE__
#define __CIRCLE__

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <iostream>
#include "Shader.h"

class Circle
{
protected:
    float radius;
    glm::vec3 translation;
    glm::vec4 color;
    glm::mat4 model;

public:
    Circle(float radius = 0.1f, glm::vec3 center = glm::vec3(0,0,0), glm::vec4 color = glm::vec4(0,0,0,1));
    
    void draw(Shader& shader, unsigned int VAO, unsigned int VBO, unsigned int EBO);

    void setRadius(float radius);
    void setTranslation(glm::vec3 position);
    void setColor(glm::vec4 newColor);

    glm::vec3 getPosition() const;
    float getRadius() const;
};

class Ball : public Circle
{
    unsigned int value;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    bool mayCollide;
public:
    Ball(
        unsigned int val = 2,
        glm::vec3 position = glm::vec3(-0.5, 0, -0.001),
        glm::vec3 velocity = glm::vec3(0.1, 0, 0),
        glm::vec3 accel = glm::vec3(0, 0, 0),
        glm::vec4 color = glm::vec4(0, 0, 0, 1)
    );

    void move();
    bool collideBall(Ball& ball);
    bool collideShield(Ball& shield);
    bool collideLeftWall(float pos) const;
    bool collideRightWall(float pos) const;
    bool collideTopWall(float pos) const;
    bool collideBottomWall(float pos) const;

    void setValue(unsigned int val);
    void addValue(unsigned int val);
    void multValue(unsigned int mult);

    void setVelocity(glm::vec3 velocity);
    void speedup(float mult);
    void setAcceleration(glm::vec3 accel);
    void addAcceleration(glm::vec3 accel);

    void swapMayCollide();

    glm::vec3 getVelocity() const;
    unsigned int getValue() const;
    bool MAYcollide() const;
};

const unsigned int MAX = 4096;
const float dt = 0.005;

#endif