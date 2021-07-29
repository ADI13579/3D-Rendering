#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<vector>
#include"coordinate.h"
using namespace std;

#define pi 3.14159
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000
bool Mesh = 1;//enables or disables mesh to be drawn in triangular planes
bool RASTERIZE = 0;//Enables or disables the SELF WRITTEN Rasterizing process
GLFWwindow* window;

//IS SUPPOSED TO CONTAIN THE PROPERTIES OF THE WORLD LIKE CAMERA,POINTLIGHT,etc...
namespace worldprops
{
    static coordinate3f Rot[3] = {
                               coordinate3f(1,0,0),
                               coordinate3f(0,1,0),
                               coordinate3f(0,0,1),
    };

    static coordinate3f Scale[3] = {
                               coordinate3f(1,0,0),
                               coordinate3f(0,1,0),
                               coordinate3f(0,0,1),
    };

    static coordinate3f Ortho[3] = {
                               coordinate3f(1,0,0),
                               coordinate3f(0,1,0),
                               coordinate3f(0,0,0),
    };

    static coordinate3f camera(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 1.4, 0);
    static coordinate3f pointlight(0,0,0);
};

//putpixel for coordinate2i
void putpixel(coordinate2i pixel, coordinate3f color)
{
    glColor3fv(&color.x);
    glVertexPointer(2, GL_INT, 0, &pixel.x);
    glDrawArrays(GL_POINTS, 0, 1);
}

void putpixel(vector<coordinate2i> pixel, coordinate3f color)
{
    glColor3fv(&color.x);
    glVertexPointer(2, GL_INT, 0, &pixel[0].x);
    glDrawArrays(GL_POINTS, 0, pixel.size());
}

void putpixel(coordinate3f pixel, coordinate3f color)
{
    glColor3f(color.x, color.y, color.z);
    glVertexPointer(3, GL_FLOAT, 0, &pixel.x);
    glDrawArrays(GL_POINTS, 0, 1);
}

void Bresenham_Line(coordinate2i a, coordinate2i b, coordinate3f color)
{
    vector<coordinate2i> Breshenham_Points;

    coordinate2i temp;
    int x1 = a.x, x2 = b.x;
    int y1 = a.y, y2 = b.y;
    //calculate slope and exchange x and y if the m>1
    bool SWAP = 0;
    if (abs(y2 - y1) > abs(x2 - x1))
    {
        swap(x1, y1);
        swap(x2, y2);
        SWAP = 1;
    }

    //Start from the small x
    if (x1 > x2)
    {
        swap(x1, x2);
        swap(y1, y2);
    }

    //Calculate dy dy and initial decision parameter p
    int dy = abs(y2 - y1);
    int dx = abs(x2 - x1);
    int yinc = (y2 > y1) ? 1 : -1;
    int p = 2 * dy - dx;

    while ((x2 - x1) > 0)
    {
        if (!SWAP)
            temp = coordinate2f(x1, y1);
        else
            temp = coordinate2f(y1, x1);

        if (p < 0)
            p += 2 * dy;
        else
        {
            p += 2 * (dy - dx);
            y1 += yinc;
        }
        x1 += 1;
        Breshenham_Points.push_back(temp);
    }
    putpixel(Breshenham_Points, coordinate3f());
}