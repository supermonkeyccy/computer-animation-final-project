#include "Rectangle.h"

/*
const float vertices[] = {
     0.5f,  1.0f, 0.0f,
    -0.5f,  1.0f, 0.0f,
    -0.5f, -1.0f, 0.0f,
     0.5f, -1.0f, 0.0f
};
const unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3
};
*/

Rectangle::Rectangle(float scaleX, float scaleY, float rotation, glm::vec3 translation, glm::vec4 color)
{
    this->scaleX = scaleX;
    this->scaleY = scaleY;
    this->rotation = rotation;
    this->translation = translation;
    this->model = glm::translate(glm::mat4(1.0f), this->translation);
    this->model = glm::rotate(this->model, this->rotation, glm::vec3(0.0, 0.0, 1.0));
    this->model = glm::scale(this->model, glm::vec3(this->scaleX, this->scaleY, 1.0));
    this->color = color;
}

void Rectangle::draw(Shader& shader, unsigned int VAO, unsigned int VBO, unsigned int EBO)
{
    shader.use();
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    shader.setVec4("squareColor", this->color);
    shader.setMat4("model", this->model);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Rectangle::setScale(float scaleX, float scaleY)
{
    this->scaleX = scaleX;
    this->scaleY = scaleY;
    this->model = glm::translate(glm::mat4(1.0f), this->translation);
    this->model = glm::rotate(this->model, this->rotation, glm::vec3(0.0, 0.0, 1.0));
    this->model = glm::scale(this->model, glm::vec3(this->scaleX, this->scaleY, 1.0));
}

void Rectangle::setRotation(float rotation)
{
    this->rotation = rotation;
    this->model = glm::translate(glm::mat4(1.0f), this->translation);
    this->model = glm::rotate(this->model, this->rotation, glm::vec3(0.0, 0.0, 1.0));
    this->model = glm::scale(this->model, glm::vec3(this->scaleX, this->scaleY, 1.0));
}

void Rectangle::setTranslation(glm::vec3 translation)
{
    this->translation = translation;
    this->model = glm::translate(glm::mat4(1.0f), this->translation);
    this->model = glm::rotate(this->model, this->rotation, glm::vec3(0.0, 0.0, 1.0));
    this->model = glm::scale(this->model, glm::vec3(this->scaleX, this->scaleY, 1.0));
}

void Rectangle::setColor(glm::vec4 newColor)
{
    this->color = newColor;
}

glm::vec3 Rectangle::getPosition() const
{
    return this->translation;
}

glm::vec4 Rectangle::getColor() const
{
    return this->color;
}
