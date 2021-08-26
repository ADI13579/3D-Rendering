#pragma once
#include"parser.h"
#include"Basic.h"
#include"Shader.h"
#include<stb/stb.h>
float angle;
coordinate3f pointlight(0,SCREEN_HEIGHT/2,0);

//-z is inside the screen +z is outside screen
void merge(std::vector<plane_t>& left, std::vector<plane_t>& right, std::vector<plane_t>& bars);
void sort(std::vector<plane_t>& bar);

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

GLfloat rotationX = 0.0f;
GLfloat rotationY = 0.0f;

Camera mycamera(coordinate3f(0, 0, 0));
Shader myshader;
bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = SCREEN_WIDTH / 2.0;
float lastY = SCREEN_HEIGHT / 2.0;
float fov = 100.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//dont process further if the plane lies completely outside
bool liesOutside(plane_t plane)
{
    //IF THE SMALLEST y OF THE THREE COORDINATES LIES OUTSIDE ->PLANE IS OUTSIDE WINDOW
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

//first just add the selcted planes to processed through camera so that threading can be efficient later
void backface_elimination(std::vector<plane_t>& planes)
{
    std::vector<plane_t> selected;
    for (auto i : planes)
    {
        if (((mycamera.Front) ^ i.centroidNormal) <= 0)
        {
            selected.push_back(i);
        }
    }
    planes.clear();
    planes.insert(planes.begin(), selected.begin(), selected.end());
}

void CameraView(std::vector<plane_t>& planes)
{
    std::vector<plane_t> selected;
    for (auto i : planes)
    {
        i.diffuseIntensities(pointlight);
        i.specularIntensities(pointlight,mycamera.Position);
        i.attenuate(pointlight);
        i.ambientIntensities(1);
        i = myshader.getShadedPlane(i);
        if (!liesOutside(i))
        {
            selected.push_back(i);
        }
    }
    planes.clear();
    planes.insert(planes.begin(), selected.begin(), selected.end());
}

int main()
{
    coordinate3f scalefactor;
    coordinate3f pivot;
    coordinate3f sky(36 / 255.0, 34 / 255.0, 34 / 255.0);

    GLFWwindow* window; //handle for the main drawable window 
    std::vector<texture> tex(200);
    //std::string model = "test";
    std::string model = "model";

    std::vector<plane_t> planes = parser::parse(model,tex,scalefactor);
    
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
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1000);
 
    glEnable(GL_POINT_SMOOTH);
    for (auto i : planes)
        for (auto j : i.v)
            pivot = pivot + j;

    pivot = pivot / (planes.size() * 3);
    texture skytex("desert-sky-1.jpg");
    skytex.load();
    std::vector<plane_t> processed;
    std::cout << planes.size();

    glBegin(GL_POINTS);
    while (!glfwWindowShouldClose(window))
    {
        processed.clear();
        processed.resize(planes.size());
        processed = planes;
        
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // set the projection matrix;
        float projMat[4][4] = { {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0} }; // initialization, contains no meaning at all 
        mycamera.GetPerspectiveMatrix(radian(mycamera.Zoom), (float)screenWidth / (float)screenHeight, 0.1, 100, projMat);
        myshader.setMat("projection", projMat);
        //show_matrix(projMat);

        // set the model matrix
        float modelMat[4][4] = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
        myshader.setMat("model", modelMat);
        // set the view matrix
        float viewMat[4][4];  //= { {1,3,2,0},{3,1,4,0},{5,0,1,7},{2,0,1,1} };
        mycamera.GetViewMatrix(viewMat);
        myshader.setMat("view", viewMat);
        for (int i = 0; i < processed.size(); i++)
            processed[i].rotate(angle, pivot);

        //method 1
        //backface_elimination(processed);
        CameraView(processed);

        std::vector<std::vector<coordinate3f>> pixelbuffer(SCREEN_HEIGHT + 1, std::vector<coordinate3f>(SCREEN_WIDTH + 1, sky));
        for (int i = 0; i < SCREEN_HEIGHT; i++)
            for (int j = 0; j < SCREEN_WIDTH; j++)
                pixelbuffer[i][j] = skytex.imagedata[i][j];

        std::vector<std::vector<float>> Zbuffer(SCREEN_HEIGHT + 1, std::vector<float>(SCREEN_WIDTH + 1, INT_MIN));
        for (auto i : processed)
        {
         //   i.scale(scalefactor.x, scalefactor.y, scalefactor.z);
            i.draw(0, Zbuffer, pixelbuffer);
        }

        //till this point colour of pixels are maintained in a 2D array called pixelbuffer
        for (int y = 0; y < pixelbuffer.size(); y++)
        {
            for (int x = 0; x < pixelbuffer[0].size(); x++)
            {
                    glColor3fv(&pixelbuffer[y][x].x);
                    glVertex2i(x,y);
            }
        }
        
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        std::cout << "FPS:" << 1 / (glfwGetTime() - currentFrame) << std::endl;
    }
    glEnd();
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
                mycamera.ProcessKeyboard(LEFT, deltaTime * 10);
                break;
            case GLFW_KEY_D:
                mycamera.ProcessKeyboard(RIGHT, deltaTime * 10);
                break;
            case GLFW_KEY_R:
                angle++;
                break;
            case GLFW_KEY_T:
                angle--;
                break;
            case GLFW_KEY_8:
                pointlight.z-=10;
                pointlight.print();
                break;
            case GLFW_KEY_2:
                pointlight.z+=10;
                break;
            case GLFW_KEY_4:
                pointlight.x-=10;
                break;
            case GLFW_KEY_6:
                pointlight.x+=10;
                break;
            case GLFW_KEY_1:
                pointlight.y+=10;
                break;
            case GLFW_KEY_0:
                pointlight.y-=10;
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