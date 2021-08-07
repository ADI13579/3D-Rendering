#pragma once
#include"parser.h"
#include"Basic.h"
#include"Shader.h"
#include<thread>

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

float lastX = SCREEN_WIDTH;
float lastY = SCREEN_HEIGHT;
bool firstMouse = true;

void ClearWindow()
{
    glClearColor(sky.x, sky.y, sky.z, 0.50);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glEnable(GL_POINT_SMOOTH);
    glPointSize(1);
}

//dont process further if the plane lies completely outside
bool liesOutside(plane_t plane)
{
    //IF THE LOWEST y OF THE THREE COORDINATES LIES OUTSIDE ->PLANE IS OUTSIDE WINDOW
    //IF THE LARGEST y OF THE THREE COORDINATES LIES OUTSIDE ->PLANE IS OUTSIDE WINDOW
    plane.sort();
    if (plane.v[0].y > SCREEN_HEIGHT || plane.v[2].y < 0)
        return 1;
    //SIMILAR TO ABOVE

    std::vector<float> X = { plane.v[0].x,plane.v[1].x,plane.v[2].x };
    std::sort(X.begin(), X.end());
    if (X[0] > SCREEN_WIDTH || X[2] < 0)
    {
        return 1;
    }
    return 0;
}

//if the surface is visible find process it through camera and finally draw
void backFaceCull_CameraView(std::vector<plane_t>& planes)
{
    std::vector<plane_t> selected;
    for (auto i : planes)
    {
        if (((mycamera.Front) ^ i.centroidNormal) <= 0)
        {
            i.diffuseIntensities(pointlight);
            i.specularIntensities();
            i = myshader.getShadedPlane(i);
            if (!liesOutside(i))
            {
                selected.push_back(i);
            }
        }
    }
    planes.clear();
    planes.insert(planes.begin(), selected.begin(), selected.end());
}

const float Setzbuffer[SCREEN_HEIGHT + 1][SCREEN_WIDTH + 1] = { INT_MIN };

int main()
{
    GLFWwindow* window; //handle for the main drawable window 
    std::vector<plane_t> planes = parser::parse("Cube");
    sort(planes);
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
    glEnable(GL_BLEND);

    std::vector<plane_t> set1(planes.begin(), planes.begin() + planes.size() / 2);
    std::vector<plane_t> set2(planes.begin() + planes.size() / 2, planes.end());

    std::vector<plane_t> part1(set1.size());
    std::vector<plane_t> part2(set2.size());
    std::vector<plane_t> processed(planes.size());
    int angle = 0;

    while (!glfwWindowShouldClose(window))
    {
        std::memcpy(Zbuffer, Setzbuffer, sizeof(Zbuffer));
        processed.clear();
        processed.resize(planes.size());
        
        ClearWindow();
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // set the projection matrix;
        float projMat[4][4] = { {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0} };
        mycamera.GetPerspectiveMatrix(radian(45), 1, 0.1, 100, projMat);
        myshader.setMat("projection", projMat);
        //show_matrix(projMat);


        // set the model matrix
        float modelMat[4][4] = { {1,0,0,-400},{0,1,0,-400},{0,0,1,-1000},{0,0,0,1} };
        myshader.setMat("model", modelMat);
        // set the view matrix
        float viewMat[4][4];  //= { {1,3,2,0},{3,1,4,0},{5,0,1,7},{2,0,1,1} };
        mycamera.GetViewMatrix(viewMat);
        myshader.setMat("view", viewMat);

        processed = planes;
        backFaceCull_CameraView(processed);
        std::vector<std::vector<int>> Zbuffer(SCREEN_HEIGHT+1,std::vector<int>(SCREEN_WIDTH+1,INT_MIN));
        
        /*for (auto i = 0; i < processed.size(); i++)
        {
            processed[i].diffuseIntensities(pointlight);
            processed[i].rotate(angle);
            processed[i].calculateCentroid();
            processed[i].specularIntensities();
        }*/

      
        for (auto i : processed)
        {
            i.draw(0,Zbuffer);
        }

        glPopMatrix();
        glfwSwapBuffers(window);
        glfwPollEvents();
        std::cout << "FPS:" << 1 / (glfwGetTime() - currentFrame) << std::endl;

        angle++;
        angle = angle % 360;

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

        case GLFW_KEY_A:
            std::cout << "deltatime" << deltaTime;
            mycamera.ProcessKeyboard(LEFT, deltaTime * 3);
            break;
        case GLFW_KEY_D:
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
        if (left[j].centroid.z < right[k].centroid.z) {
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