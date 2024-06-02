#include "Player.h"

Player::Player(glm::vec4 color, glm::vec3 shield_position)
{
    this->color = color;
    if (color == glm::vec4(0.0f, 0.75f, 0.4f, 1.0f))
        sign = SIGN::GREEN;
    else if (color == glm::vec4(0.75f, 0.4f, 0.0f, 1.0f))
        sign = SIGN::ORANGE;
    else if (color == glm::vec4(0.0f, 0.75f, 0.75f, 1.0f))
        sign = SIGN::CYAN;
    else if (color == glm::vec4(0.6f, 0.0f, 0.8f, 1.0f))
        sign = SIGN::PURPLE;

    controlBalls = std::vector<Ball>(1, Ball());
    controlBalls[0].setColor(color);

    shield.setColor(color);
    shield.setValue(1024);
    shield.setRadius(value_radius(1024));
    shield.setTranslation(shield_position);

    cannons.clear();
    bullets.clear();
    active.clear();
    numActiveBullets.clear();
}

double gravityConstant = 0.0;

void Player::draw(
    Shader& shader,
    unsigned int VAOc, unsigned int VBOc, unsigned int EBOc,
    unsigned int VAOr, unsigned int VBOr, unsigned int EBOr
)
{
    shield.draw(shader, VAOc, VBOc, EBOc);
    for (Ball& controlBall : controlBalls)
        controlBall.draw(shader, VAOc, VBOc, EBOc);
    for (unsigned int i = 0; i < cannons.size(); i++)
        cannons[i].draw(shader, VAOc, VBOc, EBOc);
    for (unsigned int i = 0; i < bullets.size(); i++)
    {
        unsigned int n = bullets[i].size();
        for (unsigned int j = 0; j < n; j++)
            if (active[i][j]) bullets[i][j].draw(shader, VAOc, VBOc, EBOc);
    }
}

void Player::setup()
{
    for (Ball& controlBall : controlBalls)
    {
        controlBall.move();
        controlBallCollideWall(controlBall);
    }

    for (Ball& cannon : cannons)
    {
        cannon.move();
        collidePlaygroundWall(cannon);
    }

    for (unsigned int i = 0; i < bullets.size();i++)
    {
        unsigned int n = bullets[i].size();
        for (unsigned int j = 0; j < n; j++)
            if (active[i][j])
            {
                bullets[i][j].move();
                collidePlaygroundWall(bullets[i][j]);
            }
    }

    selfCollision();
}

void Player::collision(Player& p)
{
    // control balls in control board
    for (Ball& controlBall : controlBalls)
    {
        if (!controlBall.MAYcollide()) continue;
        for (unsigned int i = 0; i < p.getNumControlBalls(); i++)
            if (p.controlBalls[i].MAYcollide())
                controlBall.collideBall(p.controlBalls[i]);
    }
    
    // cannons in playground
    for (int i = 0; i < this->cannons.size(); i++)
        for (int j = 0; j < p.cannons.size(); j++)
            if (this->cannons[i].collideBall(p.cannons[j]))
            {
                int val = this->cannons[i].getValue() - p.cannons[j].getValue();
                if (val > 0) {
                    this->cannons[i].setValue(val);
                    this->cannons[i].setRadius(value_radius(val));
                    p.cannons.erase(p.cannons.begin() + j);
                    j--;
                }
                else if (val < 0) {
                    p.cannons[j].setValue(-val);
                    p.cannons[j].setRadius(value_radius(-val));
                    this->cannons.erase(this->cannons.begin() + i);
                    i--;
                    break;
                }
                else {
                    this->cannons.erase(this->cannons.begin() + i);
                    p.cannons.erase(p.cannons.begin() + j);
                    i--;
                    break;
                }
            }
    
    // between cannons and a shield in the corner of the playground
    for (unsigned int i = 0; i < this->cannons.size();)
        if (this->cannons[i].collideShield(p.shield))
        {
            int val = (int)this->cannons[i].getValue() - (int)p.shield.getValue();
            if (val > 0) {
                this->cannons[i].setValue(val);
                this->cannons[i].setRadius(value_radius(val));
                p.shield.setValue(0);
                p.shield.setRadius(0.01);
                i++;
            }
            else {
                this->cannons.erase(this->cannons.begin() + i);
                p.shield.setValue(-val);
                p.shield.setRadius(value_radius(-val));
            }
        }
        else i++;
    
    for (unsigned int i = 0; i < p.cannons.size();)
        if (p.cannons[i].collideShield(this->shield))
        {
            int val = static_cast<int>(p.cannons[i].getValue()) - static_cast<int>(this->shield.getValue());
            if (val > 0) {
                p.cannons[i].setValue(val);
                p.cannons[i].setRadius(value_radius(val));
                this->shield.setValue(0);
                this->shield.setRadius(0.01);
                i++;
            }
            else {
                p.cannons.erase(p.cannons.begin() + i);
                this->shield.setValue(-val);
                this->shield.setRadius(value_radius(-val));
            }
        }
        else i++;
}

void Player::selfCollision()
{
    // control balls in control board
    for (int i = 0; i < this->controlBalls.size(); i++)
    {
        if (!controlBalls[i].MAYcollide()) continue;
        for (int j = i + 1; j < this->controlBalls.size(); j++)
            if (controlBalls[j].MAYcollide())
                controlBalls[i].collideBall(controlBalls[j]);
    }

    // cannons in playground
    for (int i = 0; i < this->cannons.size(); i++)
        for (int j = i + 1; j < this->cannons.size(); j++)
            cannons[i].collideBall(cannons[j]);
}

void Player::collideMultiplier(Ball& mult)
{
    for (int i = 0; i < controlBalls.size(); i++)
        if (this->controlBalls[i].MAYcollide())
        {
            if (this->controlBalls[i].collideBall(mult))
                this->controlBalls[i].multValue(mult.getValue());
        }
        else
        {
            glm::vec3 posDiff = this->controlBalls[i].getPosition() - mult.getPosition();
            float dist = this->controlBalls[i].getRadius() + mult.getRadius();
            if (4.0 * posDiff.x * posDiff.x + posDiff.y * posDiff.y < dist * dist)
                this->controlBalls[i].multValue(mult.getValue());
        }
}

void Player::occupySquares(Rectangle* grid)
{
    for (int ic = 0; ic < cannons.size();)
    {
        Ball& cannon = cannons[ic];
        glm::vec3 center = cannon.getPosition();
        float radius = cannon.getRadius(),
            left = center.x - radius * 0.5, right = center.x + radius * 0.7,
            top = center.y + radius, bottom = center.y - radius * 1.35;
        unsigned int value = cannon.getValue();

        // h : horizontal index, v : vertical index
        // 0.5 + 8/640.0 * (h-32) > left , h > (left -0.5) * 80 + 32
        // 0.5 + 8/640.0 * (h-32) < right, h < (right-0.5) * 80 + 32
        // 8/320.0 * (32-v) < top        , v < 32 - top    * 40
        // 8/320.0 * (32-v) > bottom     , v > 32 - bottom * 40
        int l = glm::max(glm::ceil<int>((left - 0.5) * 80 + 32), 0.0);
        int r = glm::min(glm::floor<int>((right - 0.5) * 80 + 32), 64.0);
        int t = glm::max(glm::floor<int>(32 - top * 40), 0.0);
        int b = glm::min(glm::ceil<int>(32 - bottom * 40), 64.0);

        for (int i = t; i < b && value > 0; i++)
        {
            for (int idx = i * 64 + l; idx < i * 64 + r && value > 0; idx++)
            {
                glm::vec3 posDiff = center - grid[idx].getPosition();
                if (this->color != grid[idx].getColor())
                {
                    grid[idx].setColor(this->color);
                    value--;
                }
            }
        }

        if (value <= 0)
            cannons.erase(cannons.begin() + ic);
        else
        {
            cannon.setRadius(value_radius(value));
            cannon.setValue(value);
            ic++;
        }
    }

    for (unsigned int i = 0; i < bullets.size();)
    {
        unsigned int n = bullets[i].size();
        for (unsigned int j = 0; j < n; j++)
            if (active[i][j])
            {
                glm::vec3 center = bullets[i][j].getPosition();
                int h = (center.x - 0.5) * 80 + 32;
                int v = 32 - center.y * 40;
                if (h > 63) h = 63;
                else if (h < 0) h = 0;
                if (v > 63) v = 63;
                else if (v < 0) v = 0;
                int idx = v * 64 + h;
                if (grid[idx].getColor() != this->color)
                {
                    grid[idx].setColor(this->color);
                    active[i][j] = false;
                    numActiveBullets[i]--;
                }
            }

        if (numActiveBullets[i] == 0)
        {
            bullets.erase(bullets.begin() + i);
            active.erase(active.begin() + i);
            numActiveBullets.erase(numActiveBullets.begin() + i);
        }
        else i++;
    }
}

void Player::controlBallCollideWall(Ball& controlBall)
{
    if (controlBall.collideLeftWall(-0.885))
    {
        glm::vec3 vel = controlBall.getVelocity();
        controlBall.setVelocity(glm::vec3(-vel.x, vel.y, vel.z));

        float y = controlBall.getPosition().y;
        float r = value_radius(controlBall.getValue());
        if (0.1 <= y - r && y + r <= 0.7)
        {
            reinforceShield(controlBall.getValue());
            controlBall.setValue(2);
        }
        else if (-0.6 <= y - r && y + r <= -0.2)
        {
            showerShot(controlBall.getValue());
            controlBall.setValue(2);
        }
    }
    if (controlBall.collideRightWall(-0.115))
    {
        glm::vec3 vel = controlBall.getVelocity();
        controlBall.setVelocity(glm::vec3(-vel.x, vel.y, vel.z));

        float y = controlBall.getPosition().y;
        float r = value_radius(controlBall.getValue());
        if (0.1 <= y - r && y + r <= 0.7)
        {
            bigShot(controlBall.getValue());
            controlBall.setValue(2);
        }
        else if (-0.6 <= y - r && y + r <= -0.2)
        {
            freeSqrtBigShot(controlBall.getValue());
        }
    }
    if (controlBall.collideTopWall(0.76))
    {
        glm::vec3 vel = controlBall.getVelocity();
        controlBall.setVelocity(glm::vec3(vel.x, -vel.y, vel.z));

        float x = controlBall.getPosition().x;
        if (-0.775 <= x && x <= -0.625)
        {
            // at most 3 control balls
            if (controlBalls.size() < 3) addControlBall();
        }
        else if (-0.375 <= x && x <= -0.225)
        {
            // value = 2 ^ ceil( log2(value)/2 )
            unsigned int power = glm::ceil<int>(log2(controlBall.getValue()) / 2.0);
            controlBall.setValue(1U << power);
        }
    }
    if (controlBall.collideBottomWall(-0.76))
    {
        glm::vec3 vel = controlBall.getVelocity();
        controlBall.setVelocity(glm::vec3(vel.x, -vel.y, vel.z));

        float x = controlBall.getPosition().x;
        if (-0.775 <= x && x <= -0.625)
        {
            // ghost ball : speed up 1.2x & have no collision with others
            // if the control ball is a ghost ball, set it to be not, and vice versa
            controlBall.swapMayCollide();
        }
        else if (-0.375 <= x && x <= -0.225)
        {
            // increase gravity : -0.025 ~ 0.025
            gravityConstant += 0.0025;
            if (gravityConstant > 0.025)
                gravityConstant = -0.025;
        }
    }
}

void Player::collidePlaygroundWall(Ball& cannon)
{
    if (cannon.collideLeftWall(0.1))
    {
        glm::vec3 vel = cannon.getVelocity();
        cannon.setVelocity(glm::vec3(-vel.x * 1.02, vel.y * 1.02, vel.z));
    }
    if (cannon.collideRightWall(0.9))
    {
        glm::vec3 vel = cannon.getVelocity();
        cannon.setVelocity(glm::vec3(-vel.x * 1.02, vel.y * 1.02, vel.z));
    }
    if (cannon.collideTopWall(0.8))
    {
        glm::vec3 vel = cannon.getVelocity();
        cannon.setVelocity(glm::vec3(vel.x * 1.02, -vel.y * 1.02, vel.z));
    }
    if (cannon.collideBottomWall(-0.8))
    {
        glm::vec3 vel = cannon.getVelocity();
        cannon.setVelocity(glm::vec3(vel.x * 1.02, -vel.y * 1.02, vel.z));
    }
}

void Player::computeGravity(Player& p)
{
    for (Ball& cannon1 : this->cannons)
        for (Ball& cannon2 : p.cannons)
        {
            glm::vec3 posDiff = cannon1.getPosition() - cannon2.getPosition();
            glm::vec3 direction = glm::normalize(posDiff);
            float length = posDiff.x / direction.x;
            // float F = G * cannon1.getValue() * cannon2.getValue() / length / length;
            // a1 = direction * (F / cannon1.getValue());
            // a2 = direction * (F / cannon2.getValue());
            float t = gravityConstant / length / length;
            cannon1.addAcceleration(-direction * (float)(t * sqrt(cannon2.getValue())));
            cannon2.addAcceleration( direction * (float)(t * sqrt(cannon1.getValue())));
        }
}

void Player::resetAcceleration()
{
    for (Ball& cannon : cannons)
        cannon.setAcceleration(glm::vec3(0.0));
}

void Player::showerShot(unsigned int amount)
{
    glm::vec3 v = this->shield.getPosition();
    bullets.push_back(std::vector<Ball>(amount,
        Ball(1, glm::vec3(v.x * 0.99 + 0.005, v.y * 0.99, -0.002),
            glm::vec3(0.0), glm::vec3(0.0), this->color * glm::vec4(1.25, 1.25, 1.25, 1.0))
    ));
    active.push_back(std::vector<bool>(amount, true));
    numActiveBullets.push_back(amount);

    if (amount <= 60U)
    {
        for (int i = 0, j = 45 - amount / 2; i < amount; i++, j++)
            bullets.back()[i].setVelocity(glm::vec3(cos(j * deg2rad), sin(j * deg2rad), 0) * (float)0.3);
    }
    else
    {
        float dtheta = 60.0 / amount * deg2rad, j = 15.0 * deg2rad;
        for (unsigned int i = 0; i < amount; i++, j += dtheta)
            bullets.back()[i].setVelocity(glm::vec3(cos(j), sin(j), 0) * (float)0.3);
    }
}

void Player::bigShot(unsigned int val)
{
    float r = value_radius(val);
    glm::vec3 pos = this->shield.getPosition();
    pos += (glm::vec3(0.5, 0, -0.002) - pos) * r * (float)sqrt(2);

    float angle = rand() % 91 * deg2rad;
    glm::vec3 vel = glm::vec3(cos(angle), sin(angle), 0) * (float)0.2;
    cannons.push_back(Ball(val, pos, vel, glm::vec3(0.0), this->color));
    cannons.back().setRadius(r);
}

void Player::freeSqrtBigShot(unsigned int val)
{
    bigShot(static_cast<unsigned int>(sqrt(val)));
}

void Player::reinforceShield(unsigned int val)
{
    shield.addValue(val);
    shield.setRadius(value_radius(shield.getValue()));
}

void Player::addControlBall()
{
    glm::vec3 pos(0.0), vel(0.0);
    if (sign == SIGN::GREEN)
    {
        pos = glm::vec3(-0.55, 0.05, -0.001);
        vel = normalize(glm::vec3(1, -2, 0));
    }
    else if (sign == SIGN::ORANGE)
    {
        pos = glm::vec3(-0.45, 0.05, -0.001);
        vel = normalize(glm::vec3(1, 5, 0));
    }
    else if (sign == SIGN::CYAN)
    {
        pos = glm::vec3(-0.55, -0.05, -0.001);
        vel = normalize(glm::vec3(-3, -1, 0));
    }
    else //  sign == SIGN::PURPLE
    {
        pos = glm::vec3(-0.45, -0.05, -0.001);
        vel = normalize(glm::vec3(-1, 1, 0));
    }
    
    this->controlBalls.push_back(Ball(2, pos, vel, glm::vec3(0.0), this->color));
}

glm::vec3 Player::normalize(glm::vec3 v)
{
    float mag = sqrt(v.x * v.x + v.y * v.y + v.z * v.z) * 2;
    return glm::vec3(v.x / mag, v.y / mag, v.z / mag);
}

float Player::value_radius(unsigned int val)
{
    return glm::pow(val, 0.125) / 30;
}

Ball& Player::getControlBall(unsigned int index)
{
    return controlBalls[index];
}

unsigned int Player::getNumControlBalls() const
{
    return controlBalls.size();
}

unsigned int Player::getNumCannons() const
{
    return cannons.size();
}

void Player::setColor(glm::vec4 color)
{
    this->color = color;
    if (color == glm::vec4(0.0f, 0.75f, 0.4f, 1.0f))
        sign = SIGN::GREEN;
    else if (color == glm::vec4(0.75f, 0.4f, 0.0f, 1.0f))
        sign = SIGN::ORANGE;
    else if (color == glm::vec4(0.0f, 0.75f, 0.75f, 1.0f))
        sign = SIGN::CYAN;
    else if (color == glm::vec4(0.6f, 0.0f, 0.8f, 1.0f))
        sign = SIGN::PURPLE;

    shield.setColor(color);
    for (Ball& controlBall : controlBalls)
        controlBall.setColor(color);
}

void Player::setBasePosition(glm::vec3 position)
{
    shield.setTranslation(position);
}

void Player::initializeControlBall(glm::vec3 position, glm::vec3 velocity)
{
    controlBalls[0].setValue(2);
    controlBalls[0].setTranslation(position);
    controlBalls[0].setVelocity(velocity);
}
