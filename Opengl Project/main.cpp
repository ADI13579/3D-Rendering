#pragma once
#include"parser.h"
#include"Basic.h"
#include"Shader.h"
#include<stb/stb.h>
float angle;
coordinate3f pointlight(SCREEN_WIDTH,SCREEN_HEIGHT,0);
coordinate3f scalefactor;

//-z is inside the screen +z is outside screen
void merge(std::vector<plane_t>& left, std::vector<plane_t>& right, std::vector<plane_t>& bars);
void sort(std::vector<plane_t>& bar);

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

GLfloat rotationX = 0.0f;
GLfloat rotationY = 0.0f;

Camera mycamera;
Shader myshader;
bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = SCREEN_WIDTH / 2.0;
float lastY = SCREEN_HEIGHT / 2.0;
float fov = 40.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//dont process further if the plane lies completely outside
bool liesOutside(plane_t plane)
{
    //IF THE SMALLEST y OF THE THREE COORDINATES LIES OUTSIDE ->PLANE IS OUTSIDE WINDOW
    //IF THE LARGEST y OF THE THREE COORDINATES LIES OUTSIDE ->PLANE IS OUTSIDE WINDOW
    plane.sort();
    if (plane.v[0].y > SCREEN_HEIGHT-1 || plane.v[2].y < 0)
        return 1;

    //SIMILAR TO ABOVE
    std::vector<float> X = { plane.v[0].x,plane.v[1].x,plane.v[2].x };
    std::sort(X.begin(), X.end());
    if (X[0] > SCREEN_WIDTH-1 || X[2] < 0)
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
        i.calculateIntensities(pointlight,mycamera.Position);
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
    coordinate3f pivot;
    //coordinate3f sky(36 / 255.0, 34 / 255.0, 34 / 255.0);
    coordinate3f sky(128 / 255.0, 189 / 255.0, 229 / 255.0);
    GLFWwindow* window; //handle for the main drawable window 
    std::vector<texture> tex(200);
    std::string model = "temp";
    std::vector<coordinate3f> vertices;
    std::vector<plane_t> planes = parser::parse(model,tex,scalefactor,vertices);
    
    //myshader.scale[0][0] = scalefactor.x;
    //myshader.scale[1][1] = scalefactor.y;
    //myshader.scale[2][2] = scalefactor.z;
    
    if (planes.size() == 0)
    {
        std::cout << "Nothing to draw";
        return -1;
    }

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ghantaghar", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    //glfwSetKeyCallback(window, keyCallback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

        //input 
        //keyCallback(window);
        //----------
        // set the projection matrix;
        float projMat[4][4] = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} }; // initialization, contains no meaning at all 
        mycamera.GetPerspectiveMatrix(radian(mycamera.Zoom), (float)screenWidth / (float)screenHeight, 0.1, 100, projMat);
        myshader.setMat("projection", projMat);
        //show_matrix(projMat);

        // set the model matrix
        float modelMat[4][4] = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
        myshader.setMat("model", modelMat);
        // set the view matrix
        float viewMat[4][4] = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
        //std::cout << "x:" << mycamera.Front.x << "y:" << mycamera.Front.y << "z:" << mycamera.Front.z;
        mycamera.GetViewMatrix(viewMat);
        myshader.setMat("view", viewMat);
        show_matrix(viewMat);
        for (int i = 0; i < processed.size(); i++)
        {
            processed[i].rotate(angle, pivot);
        }
        //method 1
        //backface_elimination(processed);
        CameraView(processed);

        std::vector<std::vector<coordinate3f>> pixelbuffer(SCREEN_HEIGHT+1, std::vector<coordinate3f>(SCREEN_WIDTH+1, sky));


        std::vector<std::vector<float>> Zbuffer(SCREEN_HEIGHT+1, std::vector<float>(SCREEN_WIDTH+1, INT_MAX));

        for (auto i : processed)
            i.draw(0, Zbuffer, pixelbuffer);

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
        //std::cout << "FPS:" << 1 / (glfwGetTime() - currentFrame) << std::endl;
    }
    glEnd();
    glDisable(GL_POINT_SMOOTH);
    glfwTerminate();
    return 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    const GLfloat rotationSpeed = 5;
    const float lightMovement = 20;


    switch (key)

    {

    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, true);
        break;

    case GLFW_KEY_A:
        //std::cout << "deltatime" << deltaTime;
        mycamera.ProcessKeyboard(LEFT, deltaTime);
        break;
    case GLFW_KEY_D:
        mycamera.ProcessKeyboard(RIGHT, deltaTime);
        break;

    case GLFW_KEY_W:
        //std::cout << "deltatime" << deltaTime;
        //std::cout << "PositionW" << mycamera.Position.x << "," << mycamera.Position.y << "," << mycamera.Position.z;;
        mycamera.ProcessKeyboard(FORWARD, deltaTime);
        break;
    case GLFW_KEY_S:
        mycamera.ProcessKeyboard(BACKWARD, deltaTime);
        break;
    case GLFW_KEY_R:
        angle+=10;
        break;
    case GLFW_KEY_T:
        angle--;
        break;
    case GLFW_KEY_8:
        pointlight.z -= lightMovement;
        std::cout << "8 pressed";
        pointlight.print();
        break;
    case GLFW_KEY_2:
        pointlight.z +=  lightMovement;

        break;
    case GLFW_KEY_4:
        pointlight.x -=  lightMovement;
        break;
    case GLFW_KEY_6:
        pointlight.x +=  lightMovement;
        break;
    case GLFW_KEY_1:
        pointlight.y +=  lightMovement;
        break;
    case GLFW_KEY_0:
        pointlight.y -=  lightMovement;
        break;
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