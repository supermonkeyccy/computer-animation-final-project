#include "Circle.h"

/*
const float num = sqrt(3.0) / 2;
const float polygonVertices[] = {
    0.5   , 0   , 0,
    num / 2 , 0.5 , 0,
    0.25  , num , 0,
    0     , 1   , 0,
    -0.25 , num , 0,
    -num / 2, 0.5 , 0,
    -0.5  , 0   , 0,
    -num / 2, -0.5, 0,
    -0.25 , -num, 0,
    0     , -1  , 0,
    0.25  , -num, 0,
    num / 2 , -0.5, 0
};
const unsigned int polygonIndices[] = {
    0, 1, 2 , 0, 2, 3,
    0, 3, 4 , 0, 4, 5,
    0, 5, 6 , 0, 6, 7,
    0, 7, 8 , 0, 8, 9,
    0, 9, 10, 0, 10, 11
};
*/

Circle::Circle(float radius, glm::vec3 center, glm::vec4 color)
{
    this->radius = radius;
    this->translation = center;
    this->model = glm::translate(glm::mat4(1.0f), this->translation);
    this->model = glm::scale(this->model, glm::vec3(this->radius, this->radius, 1.0));
    this->color = color;
}

void Circle::draw(Shader& shader, unsigned int VAO, unsigned int VBO, unsigned int EBO)
{
    shader.use();
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    shader.setVec4("squareColor", this->color);
    shader.setMat4("model", this->model);
    glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
}

void Circle::setRadius(float radius)
{
    this->radius = radius > 0.08 ? 0.08 : radius;
    this->model = glm::translate(glm::mat4(1.0f), this->translation);
    this->model = glm::scale(this->model, glm::vec3(this->radius, this->radius, 1.0));
}

void Circle::setTranslation(glm::vec3 position)
{
    this->translation = position;
    this->model = glm::translate(glm::mat4(1.0f), this->translation);
    this->model = glm::scale(this->model, glm::vec3(this->radius, this->radius, 1.0));
}

void Circle::setColor(glm::vec4 newColor)
{
    this->color = newColor;
}

glm::vec3 Circle::getPosition() const
{
    return this->translation;
}

float Circle::getRadius() const
{
    return this->radius;
}
