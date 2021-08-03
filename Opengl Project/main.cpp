#pragma once
#include"parser.h"
#include"Basic.h"
#include"Shader.h"
#include"mergesort.cpp"

//-z is out of the screen +z is inside screen
void merge(std::vector<plane_t>& left, std::vector<plane_t>& right, std::vector<plane_t>& bars);
void sort(std::vector<plane_t>& bar);

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


GLfloat rotationX = 0.0f;
GLfloat rotationY = 0.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

static Shader myshader;
//camera variable shifted to basic.h

float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

void ClearWindow()
{
    glClearColor(0.58, 0.89, 0.96, 0.50);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glEnable(GL_POINT_SMOOTH);
    glPointSize(1);
}

std::vector<plane_t> backFaceCull(std::vector<plane_t> planes)
{
    std::vector<plane_t> selected;
    for (auto i : planes)
        if (((mycamera.Front*-1) ^ i.centroidNormal) <= 0)
            selected.push_back(i);
    return selected;
}

std::vector<plane_t> ViewFromCamera(std::vector<plane_t> planes)
{
    std::vector<plane_t> cameraView;
    for (auto i : planes)
        cameraView.push_back(myshader.getShadedPlane(i));
    return cameraView;
}


int main()
{
    GLFWwindow* window; //handle for the main drawable window 
    std::vector<plane_t> planes=parser::parse("Cube");
    sort(planes);
    for (int i = 0; i < planes.size(); i++)
        planes[i].translate(coordinate3f(SCREEN_WIDTH / 2, SCREEN_HEIGHT/2, -1000));
    
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


    glfwMakeContextCurrent(window);
    glViewport(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnableClientState(GL_VERTEX_ARRAY);
    

    while (!glfwWindowShouldClose(window))
    {


        ClearWindow();
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        std::cout << "FPS:" << 1 / deltaTime << std::endl;
        lastFrame = currentFrame;

        // set the projection matrix;
        float projMat[4][4] = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
        myshader.setMat("projection", projMat);

        // set the view matrix
        float viewMat[4][4];  //= { {1,3,2,0},{3,1,4,0},{5,0,1,7},{2,0,1,1} };
        mycamera.GetViewMatrix(viewMat);
        myshader.setMat("view", viewMat);

        // set the model matrix
        float modelMat[4][4] = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
        myshader.setMat("model", modelMat);
        
        //let this be as a function to make easy to work
        //later sorting is necessary for knowing which surface lies behind or front(image processing type)
        //however below two functions can be combined later to make it efficient
        //enable this for camera view


        //to be done divide planes into two parts then procees the following two portions in two threads
        std::vector<plane_t> selectedPlane = backFaceCull(planes);
        std::vector<plane_t> view = ViewFromCamera(selectedPlane);
  
       // sort(view);
        for (auto i : view)
            i.draw(0);
 
        glPopMatrix();
        glfwSwapBuffers(window);
        glfwPollEvents();
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


            case GLFW_KEY_A:
                //worldprops::camera.x -= 10;
                std::cout << "deltatime"<<deltaTime;
                mycamera.ProcessKeyboard(LEFT, deltaTime * 3);
                break;
            case GLFW_KEY_D:
                //worldprops::camera.x += 10;
                mycamera.ProcessKeyboard(RIGHT, deltaTime * 3);
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

void merge(std::vector<plane_t>& left, std::vector<plane_t>& right, std::vector<plane_t>& bars)
{
    int nL = left.size();
    int nR = right.size();
    int i = 0, j = 0, k = 0;

    while (j < nL && k < nR)
    {
        if (left[j].centroid.z > right[k].centroid.z) {
            bars[i] = left[j];
            j++;
        }
        else {
            bars[i] = right[k];
            k++;
        }
        i++;
    }
    while (j < nL) {
        bars[i] = left[j];
        j++; i++;
    }
    while (k < nR) {
        bars[i] = right[k];
        k++; i++;
    }
}

//part of above code
void sort(std::vector<plane_t>& bar) {
    if (bar.size() <= 1) return;

    int mid = bar.size() / 2;
    std::vector<plane_t> left;
    std::vector<plane_t> right;

    for (size_t j = 0; j < mid; j++)
        left.push_back(bar[j]);

    for (size_t j = 0; j < (bar.size()) - mid; j++)
        right.push_back(bar[mid + j]);

    sort(left);
    sort(right);
    merge(left, right, bar);
}