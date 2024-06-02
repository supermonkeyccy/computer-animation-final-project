#ifndef __PLAYER__
#define __PLAYER__

#include <vector>
#include <random>
#include "Circle.h"
#include "Rectangle.h"

class Player
{
    glm::vec4 color;
    enum class SIGN { GREEN, ORANGE, CYAN, PURPLE } sign;

    // control board
    std::vector<Ball> controlBalls;

    // playground
    std::vector<std::vector<Ball>> bullets;
    std::vector<std::vector<bool>> active;
    std::vector<unsigned int> numActiveBullets;
    
    std::vector<Ball> cannons;
    Ball shield;

private:
    // handle collisions
    void controlBallCollideWall(Ball& controlBall);
    void collidePlaygroundWall(Ball& cannon);
    void selfCollision();

    // special rules corresponding to zones
    void showerShot(unsigned int amount);
    void bigShot(unsigned int val);
    void freeSqrtBigShot(unsigned int val);
    void reinforceShield(unsigned int val);
    void addControlBall();
    
    // tools
    glm::vec3 normalize(glm::vec3 v);
    float value_radius(unsigned int val);

public:
    Player(
        glm::vec4 color = glm::vec4(0, 0, 0, 1),
        glm::vec3 shield_position = glm::vec3(0, 0, 0)
    );

    // move and handle the collisions of
    // control balls, cannons, and bullets with wall
    // handle self collisions (cannons)
    void setup();

    // 1. collisions between control balls in control board
    // 2. collisions between cannons in playground
    // 3. collisions between cannons and a shield in the corner of the playground
    void collision(Player& p);

    // collisions between control balls and multipliers
    void collideMultiplier(Ball& mult);

    // as a cannon or a bullet reach a square
    // set the square color to be the same as the cannon or the bullet
    void occupySquares(Rectangle* grid);

    // reset the acceleration of the cannons first
    // and then add up the acceleration from other players' cannons
    void computeGravity(Player& p);
    void resetAcceleration();

    // draw shield, control balls, cannons, and bullets on board
    void draw(
        Shader& shader,
        unsigned int VAOc, unsigned int VBOc, unsigned int EBOc,
        unsigned int VAOr, unsigned int VBOr, unsigned int EBOr
    );

    Ball& getControlBall(unsigned int index = 0);
    unsigned int getNumControlBalls() const;
    unsigned int getNumCannons() const;
    std::vector<Ball>& getCannon();

    void setColor(glm::vec4 color);
    void setBasePosition(glm::vec3 position);
    void initializeControlBall(glm::vec3 position, glm::vec3 velocity);
};

const float deg2rad = glm::pi<float>() / 180;

#endif