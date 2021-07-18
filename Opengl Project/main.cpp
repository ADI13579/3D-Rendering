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

void mydraw(model Model)
{
    float halfScreenWidth = SCREEN_WIDTH / 2;
    float halfScreenHeight = SCREEN_HEIGHT / 2;
    Model.Draw();
}

coordinate3f modelCenter=coordinate3f(SCREEN_WIDTH/2,SCREEN_HEIGHT/2,-500);
model Model = model(75);

int main(void)
{
    Model.translate(modelCenter);

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


    glfwMakeContextCurrent(window);
    glViewport(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    
    int frameCount=0;
    double currentTime,previousTime;
    currentTime = previousTime =0;
    
    while (!glfwWindowShouldClose(window))
    {
        currentTime = glfwGetTime();
        cout <<"FPS:"<< 1 / (currentTime - previousTime) << endl;
        worldprops::Rot[0].print();
        worldprops::Rot[1].print();
        worldprops::Rot[2].print();

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
                worldprops::Scale[0].x += 0.01;
                worldprops::Scale[1].y += 0.01;
                break;
            case GLFW_KEY_DOWN:
                worldprops::Scale[0].x -= 0.01;
                worldprops::Scale[1].y -= 0.01;
                break;
            case GLFW_KEY_RIGHT:
                y = !y;
                break;
            case GLFW_KEY_LEFT:
                y = !y;
                alpha *= -1;
                break;
            case GLFW_KEY_SPACE:
                worldprops::camera.z += 10;
                break;
            case GLFW_KEY_BACKSPACE:
                worldprops::camera.z -= 10;
                alpha *= -1;
                break;
            case GLFW_KEY_W:
                worldprops::camera.y += 10;
                break;
            case GLFW_KEY_S:
                worldprops::camera.y -= 10;
                break;
            case GLFW_KEY_A:
                worldprops::camera.x -= 10;
                break;
            case GLFW_KEY_D:
                worldprops::camera.x += 10;
                break;

        }
        
        worldprops::Scale[0].x = worldprops::Scale[0].x < 0 ? 0 : worldprops::Scale[0].x;
        worldprops::Scale[1].y = worldprops::Scale[1].y < 0 ? 0 : worldprops::Scale[1].y;
        worldprops::Scale[2].z = worldprops::Scale[2].z < 0 ? 0 : worldprops::Scale[2].z;
        
        /*for (int i = 0; i < Model.planes.size(); i++)
        {
            Model.planes[i] = Model.planes[i].rotate(alpha, x, y, z,worldprops::camera);
        }*/
        
        worldprops::rotate(alpha, x, y, z);
    }
}