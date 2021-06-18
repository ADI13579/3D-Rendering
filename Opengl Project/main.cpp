#define _CRT_SECURE_NO_WARNINGS
#include<windows.h>//included for programs to run with dedicated gpu
#include"sphere.h"
#include"model.h"
#include<thread>

static coordinate3f angle;
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

GLfloat rotationX = 0.0f;
GLfloat rotationY = 0.0f;

//extern "C" {
//    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
//}

//extern "C" {
//    _declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
//}

namespace worldprops
{
    static coordinate3f center(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, -500);
    static GLfloat rotationX;
    static GLfloat rotationY;
    static GLfloat rotationZ;
}

void mydraw(model Model)
{
    float halfScreenWidth = SCREEN_WIDTH / 2;
    float halfScreenHeight = SCREEN_HEIGHT / 2;
    Model.Draw();
}

model Model = model(75);
int main(void)
{
    GLFWwindow* window;
    if (!glfwInit())
    {
        return -1;
    }

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ghantaghar", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    int hours, mins, seconds;

    glfwSetKeyCallback(window, keyCallback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

    int screenWidth = SCREEN_WIDTH;
    int screenHeight = SCREEN_HEIGHT;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

    GLdouble m[4][4] = { 0 };
    m[3][3] = 1;
    for (int i = 0; i < 3; i++)
        m[i][i] = 1;
    const GLdouble* t = &m[0][0];

    glfwMakeContextCurrent(window);
    glViewport(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(&m[0][0]); 
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    Model.translate(worldprops::center);
    
    GLfloat halfScreenWidth = SCREEN_WIDTH / 2;
    GLfloat halfScreenHeight = SCREEN_HEIGHT / 2;

    
    int frameCount=0;
    double currentTime,previousTime;
    currentTime = previousTime =0;
    coordinate3f p(200, 200, 200);
    p.rotation(20, 1, 0, 0, p).print();

    while (!glfwWindowShouldClose(window))
    {
        currentTime = glfwGetTime();
        cout << 1 / (currentTime - previousTime)<<endl;

        glClearColor(1.0f, 0, 0, 0.5f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPushMatrix();

        glEnableClientState(GL_VERTEX_ARRAY);
        
        Model.Draw();
        glDisableClientState(GL_VERTEX_ARRAY);

        glPopMatrix();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        previousTime = currentTime;
    }
    glfwTerminate();
    return 0;
}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    const GLfloat rotationSpeed = 5;

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        bool x=0, y=0, z=0;
        GLfloat alpha=10;
        switch (key)
        {
            case GLFW_KEY_UP:
                x = !x;
                Model.watch->rotx+=10;
                break;
            case GLFW_KEY_DOWN:
                x = !x;
                alpha *= -1;
                Model.watch->rotx-= 10;
                break;
            case GLFW_KEY_RIGHT:
                y = !y;
                Model.watch->roty+= 10;
                break;
            case GLFW_KEY_LEFT:
                y = !y;
                alpha *= -1;
                Model.watch->roty-= 10;
                break;
            case GLFW_KEY_SPACE:
                z = !z;
                Model.watch->rotz+= 10;
                break;
            case GLFW_KEY_BACKSPACE:
                z = !z;
                alpha *= -1;
                Model.watch->rotz -= 10;
                break;
        }
        Model.watch->rotx %= 360;
        Model.watch->roty %= 360;
        Model.watch->rotz %= 360;
        Model.watch->clockCenter = Model.watch->clockCenter.rotation(alpha, x, y, z, Model.baseCenter);
        for (int i = 0; i < Model.planes.size(); i++)
        {
            Model.planes[i] = Model.planes[i].rotate(alpha,x,y,z,Model.baseCenter);
        }
    }
}