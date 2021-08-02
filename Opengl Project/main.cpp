#pragma once
#include"parser.h"
#include"Basic.h"
#include"Shader.h"
#include"camera.h"
//-z is out of the screen +z is inside screen
#define pi 3.14159

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


GLfloat rotationX = 0.0f;
GLfloat rotationY = 0.0f;


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

Shader myshader;
Camera mycamera;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

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

        // per-frame time logic
// --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        glEnable(GL_POINT_SMOOTH);
        glPointSize(1);


        // set the projection matrix;
        float projMat[4][4] = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
        myshader.setMat("projection", projMat);

        // set the view matrix
        float viewMat[4][4];
        mycamera.GetViewMatrix(viewMat);
        myshader.setMat("view", viewMat);

        // set the model matrix
        float modelMat[4][4] = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
        myshader.setMat("model", modelMat);
        // Note: For n object use above code inside loop and change the model matrix accordingly.





        currentTime = glfwGetTime();
        std::cout << "FPS:" << 1 / (currentTime - previousTime) <<std::endl;
        std::cout << temp.size();

        glClearColor(0.0f, 0, 0, 0.5f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPushMatrix();

        for (auto i : temp)
        {
            i.translate(coordinate3f(screenWidth / 2, 0, 100));

            // get the new shaded plane here!

            // i(old) ----> i(new) shaded
            i = myshader.getShadedPlane(i);

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



// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    mycamera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    mycamera.ProcessMouseScroll(yoffset);
}