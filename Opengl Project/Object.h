#pragma once
#include"Draw.h"

class Object :public Draw
{
    //calculates the radius of the cone at all stack levels
    void initstackradius()
    {
        GLfloat diff = (baseRadius - topRadius) / stackCount;
        for (int i = 0; i <= stackCount; i++)
        {
            stackRadius.push_back(baseRadius - i * diff);
        }
    }
    
    //calulates the value of sin cos only so that repititive calling isn't required
    vector<coordinate3f> unitCircle()
    {
        vector<coordinate3f> sincos;
        float theeta = 2 * pi / sectorCount;
        float angle;
        int i = 0;
        for (i = 0, angle = -pi / 4; i < sectorCount; i++, angle += theeta)
        {
            sincos.push_back(coordinate3f(
                sin(angle),
                1,
                cos(angle)
                )
            );
        }
        return sincos;
    }


    //build the vertices to be connected
    void build()
    {
        vector<coordinate3f> sincos = unitCircle();
        vector<vector<coordinate3f>> vertices;
        planes.clear();
        GLfloat incheight = height / stackCount;
        vector<coordinate3f> vert, temp1(sectorCount, baseCenter), temp2(sectorCount, topCenter);
        vertices.push_back(temp1);
        for (int i = 0; i <= stackCount; i++)
        {
            vert.clear();
            for (int j = 0; j < sectorCount; j++)
            {
                vert.push_back(
                    baseCenter + coordinate3f(
                        stackRadius[i] * sincos[j].x,
                        i * incheight * sincos[j].y,
                        stackRadius[i] * sincos[j].z)
                );
            }
            vertices.push_back(vert);
        }
        vertices.push_back(temp2);
        vector<plane> temp;
        for (int i = 0; i < vertices.size() - 1; i++)
        {
            temp=genPlane(vertices[i], vertices[i + 1],baseCenter,color);
            planes.insert(planes.end(), temp.begin(), temp.end());
        }
    }
public:
    vector<plane> planes;
    GLfloat  topRadius, baseRadius;
    coordinate3f color;
    vector<GLfloat> stackRadius;
    GLfloat height;
    int stackCount, sectorCount;
    coordinate3f baseCenter, topCenter;
    Object(GLfloat _baseRadius = 0, GLfloat _topRadius = 0, GLfloat _height = 0, int _stackCount = 1, int _sectorCount = 0, coordinate3f _baseCenter = coordinate3f(),coordinate3f _color=coordinate3f(1,1,1))
    {
        color = _color;
        topRadius = _topRadius;
        baseRadius = _baseRadius;   
        height = _height;
        stackCount = _stackCount;
        sectorCount = _sectorCount;

        baseCenter = _baseCenter;
        topCenter = baseCenter;
        topCenter.y += height;

        height = (topCenter - baseCenter).y;
        initstackradius();
        build();
    }

};