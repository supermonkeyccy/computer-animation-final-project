#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "Rectangle.h"
#include "Circle.h"
#include "Player.h"

// parameters
const int windowWidth = 1280, windowHeight = 640;
const glm::vec4 GRAY  (0.3f , 0.3f , 0.3f , 1.0f);
const glm::vec4 ORANGE(0.75f, 0.4f , 0.0f , 1.0f);
const glm::vec4 PURPLE(0.6f , 0.0f , 0.8f , 1.0f);
const glm::vec4 GREEN (0.0f , 0.75f, 0.4f , 1.0f);
const glm::vec4 CYAN  (0.0f , 0.75f, 0.75f, 1.0f);
const glm::vec4 RED   (1.0f , 0.0f , 0.0f , 1.0f);

// boards and zones and the grid
Rectangle controlBoard(0.85, 0.85, 0, glm::vec3(-0.5, 0, 0), GRAY);
Rectangle reinforceShield  (0.04, 0.30, 0, glm::vec3(-0.9,  0.4, -0.001), glm::vec4(0.7, 0.7, 0.7, 1.0));
Rectangle scatterShot      (0.04, 0.20, 0, glm::vec3(-0.9, -0.4, -0.001), glm::vec4(0.7, 0.7, 0.7, 1.0));
Rectangle bigShot          (0.04, 0.30, 0, glm::vec3(-0.1,  0.4, -0.001), glm::vec4(0.7, 0.7, 0.7, 1.0));
Rectangle freeSqrtBigShot  (0.04, 0.20, 0, glm::vec3(-0.1, -0.4, -0.001), glm::vec4(0.7, 0.7, 0.7, 1.0));
Rectangle add1Ball         (0.15, 0.04, 0, glm::vec3(-0.7,  0.8, -0.001), glm::vec4(0.7, 0.7, 0.7, 1.0));
Rectangle sqrtBall         (0.15, 0.04, 0, glm::vec3(-0.7, -0.8, -0.001), glm::vec4(0.7, 0.7, 0.7, 1.0));
Rectangle ghostBall        (0.15, 0.04, 0, glm::vec3(-0.3,  0.8, -0.001), glm::vec4(0.7, 0.7, 0.7, 1.0));
Rectangle gravityModifier  (0.15, 0.04, 0, glm::vec3(-0.3, -0.8, -0.001), glm::vec4(0.7, 0.7, 0.7, 1.0));

Rectangle playground(0.85, 0.85, 0, glm::vec3(0.5, 0, 0), GRAY);
Rectangle* grid; // 64 * 64

// 4 players
Player* players;

// multiplier balls : *2, *4, *8, *16
Ball* multipliers;
//black hole
Ball* blackhole;
//gravity direction
int gravity_dir = 0;
glm::vec3 gravity;

// vertices & order to draw a rectangle
const float rectVertices[] = {
     0.5f,  1.0f, 0.0f,
    -0.5f,  1.0f, 0.0f,
    -0.5f, -1.0f, 0.0f,
     0.5f, -1.0f, 0.0f
};

const unsigned int rectIndices[] = {
    0, 1, 3,
    1, 2, 3
};

// vertices & order to approximate a circle with polygon (12)
const float num = sqrt(3.0) / 2;

const float polygonVertices[] = {
    0.5   , 0   , 0,
    num/2 , 0.5 , 0,
    0.25  , num , 0,
    0     , 1   , 0,
    -0.25 , num , 0,
    -num/2, 0.5 , 0,
    -0.5  , 0   , 0,
    -num/2, -0.5, 0,
    -0.25 , -num, 0,
    0     , -1  , 0,
    0.25  , -num, 0,
    num/2 , -0.5, 0
};

const unsigned int polygonIndices[] = {
    0, 1, 2 , 0, 2, 3,
    0, 3, 4 , 0, 4, 5,
    0, 5, 6 , 0, 6, 7,
    0, 7, 8 , 0, 8, 9,
    0, 9, 10, 0, 10, 11
};

// draw the zones (rectangle) of special rules
void drawZones(Shader& shader, unsigned int VAO, unsigned int VBO, unsigned int EBO);

// move according to velocity (no acceleration here)
// handle collision between multiplier and wall and between multipliers
void moveMultiplierBalls();

// scale the vector meet special requirement
glm::vec3 scale(glm::vec3 v);

void frameBufferSize_callback(GLFWwindow *window, int width, int height);
//black-hole extension
void add_black_hole(GLFWwindow* window, int button, int action, int mods);
void remove_black_hole(GLFWwindow* window, int button, int action, int mods);
//change gravity direction
void change_gravity_direction(GLFWwindow* window, int key, int scancode, int action, int mods);

int main()
{
    // initialize glfw and glad ------------------------------------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, "Multiply or Release", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, frameBufferSize_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return EXIT_FAILURE;
    }
    // end of initializing glfw and glad ===============================================================

    Shader shader("Shader.vs", "Shader.fs");

    // generate buffers for drawing rectangles ----------------------------------------------
    unsigned int VAOr, VBOr, EBOr;
    glGenVertexArrays(1, &VAOr);
    glGenBuffers(1, &VBOr);
    glGenBuffers(1, &EBOr);

    glBindVertexArray(VAOr);
    glBindBuffer(GL_ARRAY_BUFFER, VBOr);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOr);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectIndices), rectIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // end of generate buffers for drawing rectangles =======================================

    // generate buffers for drawing circles ------------------------------------------------------
    unsigned int VAOc, VBOc, EBOc;
    glGenVertexArrays(1, &VAOc);
    glGenBuffers(1, &VBOc);
    glGenBuffers(1, &EBOc);

    glBindVertexArray(VAOc);
    glBindBuffer(GL_ARRAY_BUFFER, VBOc);
    glBufferData(GL_ARRAY_BUFFER, sizeof(polygonVertices), polygonVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOc);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(polygonIndices), polygonIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // end of generate buffers for drawing circles ===============================================
    
    // set the position and the color of the squares in the grid --------------------
    grid = new Rectangle[64 * 64];
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            int idx = i * 64 + j;
            grid[idx].setScale(7 / 640.0, 7 / 640.0);
            grid[idx].setTranslation(
                glm::vec3((67.5 + 8 * j) / 640.0, (252.5 - 8 * i) / 320.0, -0.001));

            if (i < 32)
            {
                if (j < 32) grid[idx].setColor(GREEN);
                else grid[idx].setColor(ORANGE);
            }
            else
            {
                if (j < 32) grid[idx].setColor(CYAN);
                else grid[idx].setColor(PURPLE);
            }
        }
    }
    // end of setting the squares in the grid =======================================

    // setup multiplier balls ----------------------------------------
    multipliers = new Ball[4];
    multipliers[0].setTranslation(glm::vec3(-0.8,  0.5, -0.001));
    multipliers[1].setTranslation(glm::vec3(-0.2,  0.5, -0.001));
    multipliers[2].setTranslation(glm::vec3(-0.8, -0.5, -0.001));
    multipliers[3].setTranslation(glm::vec3(-0.2, -0.5, -0.001));
    multipliers[0].setVelocity(scale(glm::vec3(-2,  1, 0)));
    multipliers[1].setVelocity(scale(glm::vec3( 5,  1, 0)));
    multipliers[2].setVelocity(scale(glm::vec3(-1, -3, 0)));
    multipliers[3].setVelocity(scale(glm::vec3( 1, -1, 0)));

    for (int i = 0; i < 4; i++) {
        multipliers[i].setValue(1 << (i+1));
        multipliers[i].setRadius(25.0 / sqrt(sqrt(1 << i)) / 640);
        multipliers[i].setColor(glm::vec4(0.7, 0.7, 0.7, 1.0));
    }
    // end of setup multiplier balls =================================

    // setup players -------------------------------------------------------------------------------
    // control board : control balls
    // playground    : base position
    players = new Player[4];
    players[0].setColor(GREEN);
    players[1].setColor(ORANGE);
    players[2].setColor(CYAN);
    players[3].setColor(PURPLE);
    players[0].setBasePosition(glm::vec3(0.1,  0.8, -0.002));
    players[1].setBasePosition(glm::vec3(0.9,  0.8, -0.002));
    players[2].setBasePosition(glm::vec3(0.1, -0.8, -0.002));
    players[3].setBasePosition(glm::vec3(0.9, -0.8, -0.002));
    players[0].initializeControlBall(glm::vec3(-0.55,  0.05, -0.001), scale(glm::vec3( 1, -2, 0)));
    players[1].initializeControlBall(glm::vec3(-0.45,  0.05, -0.001), scale(glm::vec3( 1,  5, 0)));
    players[2].initializeControlBall(glm::vec3(-0.55, -0.05, -0.001), scale(glm::vec3(-3, -1, 0)));
    players[3].initializeControlBall(glm::vec3(-0.45, -0.05, -0.001), scale(glm::vec3(-1,  1, 0)));
    // end of setting players ======================================================================

    // render loop ------------------------------------------------------
    glEnable(GL_DEPTH_TEST);
    glfwSetMouseButtonCallback(window, add_black_hole);
    glfwSetKeyCallback(window, change_gravity_direction);
    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.1, 0.1, 0.1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        controlBoard.draw(shader, VAOr, VBOr, EBOr);
        playground.draw(shader, VAOr, VBOr, EBOr);
        drawZones(shader, VAOr, VBOr, EBOr);
        for (int i = 0; i < 4096; i++)
            grid[i].draw(shader, VAOr, VBOr, EBOr);

        // move multiplier balls and handle the collisions
        moveMultiplierBalls();

        // reset the acceleration of cannons
        for (int i = 0; i < 4; i++)
            players[i].resetAcceleration();

        for (int i = 0; i < 4; i++)
        {
            // compute force applied on cannons
            // store the acceleration
            for (int j = i + 1; j < 4; j++)
                players[i].computeGravity(players[j]);
            //blackhole effect
            if (blackhole)
            {
                std::vector<Ball>& can = players[i].getCannon();
                for (int x = 0; x < can.size(); x++)
                {
                    glm::vec3 diff = blackhole->getPosition() - can[x].getPosition();
                    glm::vec3 dir = glm::normalize(diff);
                    double length = diff.x / dir.x;
                    double t = 0.001 / length / length;
                    can[x].addAcceleration(dir * (float)(t * sqrt(blackhole->getValue())));
                }
            }
            //additional gravity effect
            if (gravity_dir == 1 || gravity_dir == 2 || gravity_dir == 3 || gravity_dir == 4)
            {
                std::vector<Ball>& can = players[i].getCannon();
                if (gravity_dir == 1)
                {
                    gravity = { 0.0, -1.0, 0.0 };
                }
                else if (gravity_dir == 2)
                {
                    gravity = { 0.0, 1.0, 0.0 };
                }
                else if (gravity_dir == 3)
                {
                    gravity = { 1.0, 0.0, 0.0 };
                }
                else if (gravity_dir == 4) {
                    gravity = { -1.0, 0.0, 0.0 };
                }
                else
                {
                    gravity = { 0.0, 0.0, 0.0 };
                }
                for (int x = 0; x < can.size(); x++)
                {
                    can[x].addAcceleration(gravity / (float)(can[x].getValue()));
                }
            }
            // 0. move control balls, cannons, bullets
            // 1. collisions between control balls and the wall
            //    a player should perform some behavior upon reaching some zones
            // 2. collisions between cannons and the wall of playground
            // 3. collisions between bullets and the wall of playground
            players[i].setup();

            // collisions between control balls
            for (int j = i + 1; j < 4; j++)
                players[i].collision(players[j]);
            
            // collisions between control balls and multipliers
            for (int j = 0; j < 4; j++)
                players[i].collideMultiplier(multipliers[j]);

            // when a cannon or a bullet reach a square
            // the color of the square is set to the same one
            players[i].occupySquares(grid);
            players[i].draw(shader, VAOc, VBOc, EBOc, VAOr, VBOr, EBOr);
        }

        for (int i = 0; i < 4; i++)
            multipliers[i].draw(shader, VAOc, VBOc, EBOc);

        if(blackhole)
            blackhole->draw(shader, VAOc, VBOc, EBOc);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    // end of render loop ===============================================

    return 0;
}

void drawZones(Shader& shader, unsigned int VAO, unsigned int VBO, unsigned int EBO)
{
    float t = glfwGetTime();
    float r = sin(1.571 * t) / 4 + 0.6;
    float g = sin(2.000 * t) / 4 + 0.4;
    float b = sin(2.718 * t) / 4 + 0.5;

    reinforceShield.setColor(glm::vec4(r, g, b, 1));
    scatterShot.setColor(glm::vec4(r, g, b, 1));
    bigShot.setColor(glm::vec4(r, g, b, 1));
    freeSqrtBigShot.setColor(glm::vec4(r, g, b, 1));
    add1Ball.setColor(glm::vec4(r, g, b, 1));
    sqrtBall.setColor(glm::vec4(r, g, b, 1));
    ghostBall.setColor(glm::vec4(r, g, b, 1));
    gravityModifier.setColor(glm::vec4(r, g, b, 1));

    reinforceShield.draw(shader, VAO, VBO, EBO);
    scatterShot.draw(shader, VAO, VBO, EBO);
    bigShot.draw(shader, VAO, VBO, EBO);
    freeSqrtBigShot.draw(shader, VAO, VBO, EBO);
    add1Ball.draw(shader, VAO, VBO, EBO);
    sqrtBall.draw(shader, VAO, VBO, EBO);
    ghostBall.draw(shader, VAO, VBO, EBO);
    gravityModifier.draw(shader, VAO, VBO, EBO);
}

void moveMultiplierBalls()
{
    for (int i = 0; i < 4; i++)
    {
        multipliers[i].move();
        if (multipliers[i].collideLeftWall(-0.885) || multipliers[i].collideRightWall(-0.115)) {
            glm::vec3 vel = multipliers[i].getVelocity();
            multipliers[i].setVelocity(glm::vec3(-vel.x, vel.y, vel.z));
        }
        if (multipliers[i].collideTopWall(0.76) || multipliers[i].collideBottomWall(-0.76)) {
            glm::vec3 vel = multipliers[i].getVelocity();
            multipliers[i].setVelocity(glm::vec3(vel.x, -vel.y, vel.z));
        }

        for (int j = i + 1; j < 4; j++)
            multipliers[i].collideBall(multipliers[j]);
    }
}

glm::vec3 scale(glm::vec3 v)
{
    float mag = sqrt(v.x * v.x + v.y * v.y + v.z * v.z) * 2;
    return glm::vec3(v.x / mag, v.y / mag, v.z / mag);
}

void frameBufferSize_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void add_black_hole(GLFWwindow* window, int button, int action, int mods) 
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        xpos = (2.0f * xpos) / windowWidth - 1.0f;
        ypos = 1.0f - (2.0f * ypos) / windowHeight;
        std::cout << "Mouse button pressed at position (" << xpos << ", " << ypos << ")\n";
        blackhole = new Ball(16384, glm::vec3(xpos, ypos, -0.002), glm::vec3(0.0), glm::vec3(0.0), glm::vec4(0.0, 0.0, 0.0, 1.0));
        glfwSetMouseButtonCallback(window, remove_black_hole);
    }
}

void remove_black_hole(GLFWwindow* window, int button, int action, int mods) 
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        xpos = (2.0f * xpos) / windowWidth - 1.0f;
        ypos = 1.0f - (2.0f * ypos) / windowHeight;
        std::cout << "Mouse button released at position (" << xpos << ", " << ypos << ")\n";
        blackhole = NULL;
        glfwSetMouseButtonCallback(window, add_black_hole);
    }
}

void change_gravity_direction(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key) {
            case GLFW_KEY_1:
                std::cout << "change gravity from up to down.\n";
                gravity_dir = 1;
                break;
            case GLFW_KEY_2:
                std::cout << "change gravity from down to up.\n";
                gravity_dir = 2;
                break;
            case GLFW_KEY_3:
                std::cout << "change gravity from left to right.\n";
                gravity_dir = 3;
                break;
            case GLFW_KEY_4:
                std::cout << "change gravity from right to left.\n";
                gravity_dir = 4;
                break;
            case GLFW_KEY_0:
                std::cout << "no gravity.\n";
                gravity_dir = 0;
                break;
            default:
                break;
        }
    }
}


