#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include"coordinate.h"
#include <cstdlib>
#include<vector>
#define pi 3.14159
#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 1080
using namespace std;
namespace worldprops
{
    coordinate3f R[3] = {
                            coordinate3f(1,0,0),
                            coordinate3f(0,1,0),
                            coordinate3f(0,0,1),
    };
}
class plane
{
public:
    vector<coordinate3f> normal;
    vector<coordinate3f> vertex;
    coordinate3f color;
    void sort()
    {
        if (vertex[0].z < vertex[1].z)
            swap(vertex[0], vertex[1]);
        if (vertex[0].z < vertex[2].z)
            swap(vertex[0], vertex[2]);
        if (vertex[1].z < vertex[2].z)
            swap(vertex[1], vertex[2]);
    }
    plane(coordinate3f a=coordinate3f(), coordinate3f b = coordinate3f(), coordinate3f c = coordinate3f(),coordinate3f _color=coordinate3f(1,1,1))
    {
        color = _color;
        vertex = { a, b,c };

    }
    
    plane translate(coordinate3f shiftVector)
    {
        plane temp = *this;
        for (int i = 0; i < temp.vertex.size(); i++)
            temp.vertex[i] = temp.vertex[i] + shiftVector;
        return temp;
    }

    plane rotate(GLfloat alpha, bool _x, bool _y, bool _z,coordinate3f pivot=coordinate3f())
    {
        plane temp = *this;
        for (int i = 0; i < vertex.size(); i++)
        {
            temp.vertex[i] = temp.vertex[i].rotation(alpha, _x, _y, _z,pivot);
        }
        return temp;
        sort();
    }
    void drawPlane()
    {
        glVertexPointer(3, GL_FLOAT, 0, &vertex[0]);
        glColor3d(color.x, color.y, color.z);
        glDrawArrays(GL_TRIANGLES, 0, 3);      
    }

    void drawMesh(coordinate3f color=coordinate3f(0,0,0))
    {
        vector<coordinate3f> m(6, coordinate3f());
        m = {
                                 vertex[0],
                                 vertex[1],
                                 vertex[1],
                                 vertex[2],
                                 vertex[2],
                                 vertex[0]
        };
    
        glVertexPointer(3, GL_FLOAT, 0, &m[0]);
        glColor3d(color.x, color.y, color.z);
        glLineWidth(1);
        //glDrawArrays(GL_LINES, 0, 6);
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
    vector<plane> genPlane(vector<coordinate3f> vertices1, vector<coordinate3f> vertices2,coordinate3f color=coordinate3f(1,1,1))
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
