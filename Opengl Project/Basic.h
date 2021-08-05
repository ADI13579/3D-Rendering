#pragma once
#include <vector>
#include"coordinate.h"
#include"camera.h"
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000
#define PI 3.14159
static coordinate3f pointlight(SCREEN_WIDTH/2,SCREEN_HEIGHT/2,500);
static Camera mycamera(coordinate3f(0,0,0));
static coordinate3f sky(0.58, 0.89, 0.96);
static float Zbuffer[SCREEN_HEIGHT + 1][SCREEN_WIDTH + 1] = { INT_MIN };

//putpixel for coordinate2i
static void putpixel(coordinate2i pixel, coordinate3f color)
{
    glColor3fv(&color.x);
    glVertexPointer(2, GL_INT, 0, &pixel.x);
    glDrawArrays(GL_POINTS, 0, 1);
};
static void debug()
{
    std::cout << "Why Wrong"<<std::endl;
}

static void putpixel(std::vector<coordinate2i> pixel, coordinate3f color)
{
    glColor3fv(&color.x);
    glVertexPointer(2, GL_INT, 0, &pixel[0].x);
    glDrawArrays(GL_POINTS, 0, pixel.size());
}

static void putpixel(coordinate3f pixel, coordinate3f color)
{
    GLfloat a[] = { pixel.x,pixel.y,pixel.z };
    glColor3f(color.x, color.y, color.z);
    glVertexPointer(3, GL_FLOAT, 0, &a);
    glDrawArrays(GL_POINTS, 0, 1);
};

static void Bresenham_Line(coordinate2i a, coordinate2i b, coordinate3f color)
{
    std::vector<coordinate2i> Breshenham_Points;

    coordinate2i temp;
    int x1 = a.x, x2 = b.x;
    int y1 = a.y, y2 = b.y;
    //calculate slope and exchange x and y if the m>1
    bool SWAP = 0;
    if (abs(y2 - y1) > abs(x2 - x1))
    {
        std::swap(x1, y1);
        std::swap(x2, y2);
        SWAP = 1;
    }

    //Start from the small x
    if (x1 > x2)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
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
    putpixel(Breshenham_Points, color);
}


static float radian(float degree) {
    return PI / 180 * degree;
}


//static void show_matrix(float mat[4][4]) {
//	// show matrix
//
//			
//			std::cout << "Result matrix is \n";
//			for (int i = 0; i < 4; i++) {
//				for (int j = 0; j < 4; j++)
//					std::cout << mat[i][j] << " ";
//				std::cout << "\n";
//			}
//			
//}