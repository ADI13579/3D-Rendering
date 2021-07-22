#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include"coordinate.h"
#include <cstdlib>
#include<vector>
#include<algorithm>
#define pi 3.14159
#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 1080
using namespace std;
bool Mesh = 0;
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

    static coordinate3f camera(SCREEN_WIDTH /2, SCREEN_HEIGHT / 1.4, -0);
   
    void rotate(GLfloat alpha, bool _x, bool _y, bool _z)
    {
        for (int i = 0; i < 3; i++)
        {
            Rot[i] = Rot[i].rotation(alpha, _x, _y, _z);
        }
    }
}

coordinate3f pointTransform(coordinate3f t,coordinate3f pivot)
{
    t = t - pivot;
    t = coordinate3f(worldprops::Rot[0] ^ t, worldprops::Rot[1] ^ t, worldprops::Rot[2] ^ t);
    t = coordinate3f(worldprops::Scale[0] ^ t, worldprops::Scale[1] ^ t, worldprops::Scale[2] ^ t);
    t = t + pivot;
    return t;
};


int count;
void putpixel(coordinate2f pixel, coordinate3f color)
{
    glColor3f(color.x/255,color.y/255,color.z/255);
    glVertexPointer(2, GL_FLOAT, 0, &pixel.x);
    glDrawArrays(GL_POINTS, 0, 1);
}

void putpixel(coordinate3f pixel, coordinate3f color)
{
    glColor3f(color.x / 255, color.y / 255, color.z / 255);
    glVertexPointer(3, GL_FLOAT, 0, &pixel.x);
    glDrawArrays(GL_POINTS, 0, 1);
}


void Bresenham_Line(coordinate2f a, coordinate2f b, coordinate3f color)
{
    vector<coordinate2f> meshpoint = {};
    coordinate2f temp;
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
            temp=coordinate2f(x1, y1);
        else
            temp=coordinate2f(y1, x1);

        if (p < 0)
            p += 2 * dy;
        else
        {
            p += 2 * (dy - dx);
            y1 += yinc;
        }
        x1 += 1;
        putpixel(temp, color);
    }
}

int GetIntersectPoint(coordinate2f a, coordinate2f b, int x)
{
    if (b.x == a.x)
        return INT_MAX;
    else if (a.x > b.x)
        swap(a, b);
    if (x<a.x || x>b.x)
        return INT_MAX;

    float m = (b.y - a.y) / (b.x - a.x);
    float c = (a.y - m * a.x);
    int y = m * x + c;

    return y;
}

class plane
{
public:
    coordinate3f normal,centroid,vertexNormal[3];
    vector<coordinate3f> vertex;
    coordinate3f color;
    int count;
    coordinate3f ref;
    void sort()
    {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3 - 1; j++)
                if (vertex[j].z < vertex[j + 1].z)
                    swap(vertex[j], vertex[j + 1]);
    }
    void getnormal()
    {
        centroid = (vertex[0] + vertex[1] + vertex[2])/3;
        normal = (centroid - vertex[1]) * (centroid - vertex[2]);
        if (((centroid - ref) ^ normal) > 0)
            normal = coordinate3f() - normal;

        for (int i = 0; i < 3; i++)
        {
            vertexNormal[i] = (vertex[i] - vertex[(i + 1) % 3]) * (vertex[i] - vertex[(i + 2) % 3]);
            if (((vertex[i] - ref) ^ vertexNormal[i]) > 0)
                vertexNormal[i] = coordinate3f() - vertexNormal[i];
        }
    }

    plane(coordinate3f a=coordinate3f(), coordinate3f b = coordinate3f(), coordinate3f c = coordinate3f(),coordinate3f _ref=coordinate3f(),coordinate3f _color=coordinate3f(1,1,1))
    {
        ref = _ref;
        color = _color;
        vertex = { a, b,c };

        sort();
        getnormal();
    }
    
    plane scale(coordinate3f scalefactor)
    {
        plane t = *this;
        for (int i = 0; i < t.vertex.size(); i++)
            t.vertex[i] = t.vertex[i].scaling(scalefactor.x, scalefactor.y, scalefactor.z);
       t.ref = t.ref.scaling(scalefactor.x, scalefactor.y, scalefactor.z);
       t.getnormal();
        return t;
    }
    
    plane translate(coordinate3f shiftVector)
    {
        plane t = *this;
        for (int i = 0; i < t.vertex.size(); i++)
            t.vertex[i] = t.vertex[i] + shiftVector;
        t.ref = t.ref + shiftVector;
        t.sort();
        t.getnormal();
        return t;
    }

    plane transform()
    {
        plane temp = *this;
     
        for (int i = 0; i < temp.vertex.size(); i++)
        {
            coordinate3f t= temp.vertex[i];
            t = t - worldprops::camera;
            t = coordinate3f(worldprops::Rot[0] ^ t, worldprops::Rot[1] ^t, worldprops::Rot[2] ^ t);
            t = coordinate3f(worldprops::Scale[0] ^ t, worldprops::Scale[1] ^ t, worldprops::Scale[2] ^ t);
            t = t + worldprops::camera;
            temp.vertex[i] = t;
        }
        temp.ref=pointTransform(temp.ref,worldprops::camera);
        temp.sort();
        temp.getnormal();
        return temp;
    }

    plane rotate(GLfloat alpha, bool _x, bool _y, bool _z,coordinate3f pivot=coordinate3f())
    {
        plane temp = *this;
        for (int i = 0; i < vertex.size(); i++)
        {
            temp.vertex[i] = temp.vertex[i].rotation(alpha, _x, _y, _z,pivot);
        }
        temp.ref = temp.ref.rotation(alpha, _x, _y, _z, pivot);
        temp.sort();
        temp.getnormal();
        return temp;
    }

    
    
    
    void draw()
    {

        vector<coordinate2f> t  = {
                                    coordinate2f(int(vertex[0].x),int(vertex[0].y)),
                                    coordinate2f(int(vertex[1].x),int(vertex[1].y)),
                                    coordinate2f(int(vertex[2].x),int(vertex[2].y)),
        };

        if (t[0] == t[1] || t[1] == t[2] || t[0] == t[2])
            return;

        
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 3 - 1; j++)
                if (t[j].x > t[j + 1].x)
                    swap(t[j], t[j + 1]);
        
        
        int min, max;
        coordinate2f temp;
        vector<int> point;
        
        for(int x = t[0].x; x < t[2].x; x++)
        {
            temp.x = x;
            point = vector<int>{
                                        GetIntersectPoint(t[0], t[1], x),
                                        GetIntersectPoint(t[1], t[2], x),
                                        GetIntersectPoint(t[0], t[2], x),
            };

            std::sort(point.begin(), point.end());

            point.erase(std::unique(point.begin(), point.end()), point.end());

            if (point[point.size() - 1] == INT_MAX)
                point.pop_back();

            if (point.size() == 0)
                break;
            min = point[0];
            max = point[point.size() - 1];

            for (int y = min; y < max; y++)
            {
                temp.y = y;
                putpixel(temp, color);
            }
        }
      
        if (Mesh)
        {
            Bresenham_Line(vertex[0], vertex[1], coordinate3f(0, 0, 0));
            Bresenham_Line(vertex[0], vertex[2], coordinate3f(0, 0, 0));
            Bresenham_Line(vertex[1], vertex[2], coordinate3f(0, 0, 0));
        }
    }

    void print()
    {
        cout<<"==========================="<<endl;
        for (auto i : vertex)
            i.print();
    }
};

class Draw
{
public:    
    vector<plane> genPlane(vector<coordinate3f> vertices1, vector<coordinate3f> vertices2,coordinate3f ref,coordinate3f color=coordinate3f(1,1,1))
    {
        vector<plane> planes;
        int size = vertices1.size();
        
        plane tplane;
        for (int i = 0; i < size; i++)
        {
            tplane = plane(
                vertices1[i % size],
                vertices1[(i + 1) % size],
                vertices2[i % size],
                ref,       
                color
            );
            planes.push_back(tplane);
        }

        for (int i = 0; i < size; i++)
        {
            tplane = plane(
                            vertices2[i % size],
                            vertices2[(i + 1) % size],
                            vertices1[(i + 1) % size],
                            ref, 
                            color
                            );
            planes.push_back(tplane);
        }
        return planes;
    }

    void DrawPoint(vector<coordinate3f> vertices, coordinate3f color, int pointsize = 2)
    {
        int n = vertices.size();
        GLfloat* pointVertex = &vertices[0].x;
        glEnable(GL_POINT_SMOOTH);
        glPointSize(pointsize);
        glColor3d(color.x, color.y, color.z);
        glVertexPointer(3, GL_FLOAT, 0, pointVertex);
        glDrawArrays(GL_POINTS, 0, n);
        glDisable(GL_POINT_SMOOTH);
    }
};