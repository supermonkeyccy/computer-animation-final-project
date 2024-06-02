#include "Circle.h"

Ball::Ball(unsigned int val, glm::vec3 position, glm::vec3 velocity, glm::vec3 accel, glm::vec4 color)
    : Circle(25 / 640.0, position, color)
{
    this->value = val;
    if (val == 1U) radius = 0.01;
    else if (val >= 1024U) radius = 0.08;
    this->velocity = velocity;
    this->acceleration = accel;
    this->mayCollide = true;
}

void Ball::move()
{
    this->velocity += this->acceleration * glm::vec3(dt);
    this->translation += this->velocity * glm::vec3(dt);
    this->model = glm::translate(glm::mat4(1.0f), this->translation);
    this->model = glm::scale(this->model, glm::vec3(this->radius, this->radius, 1.0));
}

bool Ball::collideBall(Ball& ball)
{
    glm::vec3 posDiff = this->translation - ball.translation;
    float dist = this->radius + ball.radius;
    if (4.0 * posDiff.x * posDiff.x + posDiff.y * posDiff.y > dist * dist)
        return false;

    glm::vec3 t = ball.velocity;
    ball.velocity = this->velocity;
    this->velocity = t;
    return true;
}

bool Ball::collideShield(Ball& shield)
{
    glm::vec3 posDiff = this->translation - shield.translation;
    float dist = this->radius + shield.radius;
    if (posDiff.x * posDiff.x + posDiff.y * posDiff.y > dist * dist)
        return false;

    glm::vec3 direction = glm::normalize(this->translation - shield.translation);
    glm::vec3 vel = this->velocity;
    glm::vec3 orthoProjection = (vel.x * direction.x + vel.y * direction.y) * direction;
    this->velocity = vel - (float)2.0 * orthoProjection;

    return true;
}

bool Ball::collideLeftWall(float pos) const
{
    return this->translation.x - this->radius * 0.6 <= pos
        && this->velocity.x < 0.0;
}

bool Ball::collideRightWall(float pos) const
{
    return this->translation.x + this->radius * 0.6 >= pos
        && this->velocity.x > 0.0;
}

bool Ball::collideTopWall(float pos) const
{
    return this->translation.y + this->radius >= pos
        && this->velocity.y > 0.0;
}

bool Ball::collideBottomWall(float pos) const
{
    return this->translation.y - this->radius <= pos
        && this->velocity.y < 0.0;
}

void Ball::setValue(unsigned int val)
{
    this->value = val > MAX ? val / MAX : val;
    if (this->value < 2) this->value = 2;
}

void Ball::addValue(unsigned int val)
{
    this->value += val;
}

void Ball::multValue(unsigned int mult)
{
    this->value *= mult;
    this->value = this->value > MAX ? this->value / MAX : this->value;
}

void Ball::setVelocity(glm::vec3 velocity)
{
    this->velocity = velocity;
}

void Ball::speedup(float mult)
{
    this->velocity *= mult;
}

void Ball::setAcceleration(glm::vec3 accel)
{
    this->acceleration = accel;
}

void Ball::addAcceleration(glm::vec3 accel)
{
    this->acceleration += accel;
}

void Ball::swapMayCollide()
{
    if (this->mayCollide)
    {
        this->mayCollide = false;
        this->velocity *= (float)1.2;
    }
    else
    {
        this->mayCollide = true;
        this->velocity *= (float)(1 / 1.2);
    }
}

glm::vec3 Ball::getVelocity() const
{
    return this->velocity;
}

unsigned int Ball::getValue() const
{
    return this->value;
}

bool Ball::MAYcollide() const
{
    return this->mayCollide;
}
