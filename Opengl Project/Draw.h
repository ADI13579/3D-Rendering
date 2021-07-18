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

    vector<int> stack;
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
class plane
{
public:
    vector<coordinate3f> normal;
    vector<coordinate3f> vertex;
    coordinate3f color;
    int count;
    void sort()
    {
        for (int i = 0; i < 3; i++)
            for (int j = i; j < 3 - 1; j++)
                if (vertex[j].z < vertex[j + 1].z)
                    swap(vertex[j], vertex[j + 1]);
    }
    
    plane(coordinate3f a=coordinate3f(), coordinate3f b = coordinate3f(), coordinate3f c = coordinate3f(),coordinate3f _color=coordinate3f(1,1,1))
    {
        color = _color;
        vertex = { a, b,c };
        sort();
    }
    
    plane scale(coordinate3f scalefactor)
    {
        plane t = *this;
        for (int i = 0; i < t.vertex.size(); i++)
            t.vertex[i] = t.vertex[i].scaling(scalefactor.x, scalefactor.y, scalefactor.z);
        t.sort();
        return t;
    }
    
    plane translate(coordinate3f shiftVector)
    {
        plane t = *this;
        for (int i = 0; i < t.vertex.size(); i++)
            t.vertex[i] = t.vertex[i] + shiftVector;
        t.sort();
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
        temp.sort();
        return temp;
    }

    plane rotate(GLfloat alpha, bool _x, bool _y, bool _z,coordinate3f pivot=coordinate3f())
    {
        plane temp = *this;
        for (int i = 0; i < vertex.size(); i++)
        {
            temp.vertex[i] = temp.vertex[i].rotation(alpha, _x, _y, _z,pivot);
        }
        temp.sort();
        return temp;
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
        glDrawArrays(GL_LINES, 0, 6);
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