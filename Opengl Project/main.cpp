#pragma once
#include"parser.h"
#include"Basic.h"
//-z is out of the screen +z is inside screen
#define pi 3.14159

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main()
{
    GLFWwindow* window; //handle for the main drawable window 
    std::vector<plane_t> temp=parser::parse("Cube");
    
    if (!glfwInit())
        return -1;


    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ghantaghar", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, keyCallback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

    int screenWidth = SCREEN_WIDTH;
    int screenHeight = SCREEN_HEIGHT;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);


    double currentTime, previousTime;
    currentTime = previousTime = 0;
    glfwMakeContextCurrent(window);
    glViewport(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnableClientState(GL_VERTEX_ARRAY);
    

    while (!glfwWindowShouldClose(window))
    {
        glEnable(GL_POINT_SMOOTH);
        glPointSize(1);

        currentTime = glfwGetTime();
        std::cout << "FPS:" << 1 / (currentTime - previousTime) <<std::endl;
        std::cout << temp.size();

        glClearColor(0.0f, 0, 0, 0.5f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPushMatrix();

        for (auto i : temp)
        {
            i.translate(coordinate3f(screenWidth / 2, screenHeight/2, 100));
            i.draw(0);
        }
       
        glPopMatrix();
        glfwSwapBuffers(window);
        glfwPollEvents();
        previousTime = currentTime;
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_POINT_SMOOTH);
    glfwTerminate();
    return 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    const GLfloat rotationSpeed = 5;

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
            case GLFW_KEY_M:
                
            break;
            case GLFW_KEY_T:
                
            break;
        }
    }
}
