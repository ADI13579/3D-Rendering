#pragma once
#include"plane.h"
class Sphere
{
    vector<coordinate3f> sincos;
    vector<coordinate2f> sincosz;
    //calulates the value of sin cos only so that repititive calling isn't required
    void unitCircle()
    {
        sincos.clear();
        sincosz.clear();

        float theeta = 2 * pi / sectorCount;
        float phi = pi / stackCount;

        int i = 0;
        for (i = 0; i <= stackCount / n + 1; i++)
        {
            sincosz.push_back(coordinate2f(
                sin(phi * (i)),
                cos(phi * i)
            ));
        }

        for (i = 0; i < sectorCount; i++)
        {
            sincos.push_back(coordinate3f(
                cos(theeta * i),
                1,
                sin(theeta * i)
            )
            );
        }
    }


    //build the vertices to be connected
    void build()
    {
        vector<vector<coordinate3f>> vertices;
        planes.clear();
        vector<coordinate3f> vert,temp1(sectorCount, baseCenter);
        
        for (int i = 0; i <= stackCount / n + 1; i++)
        {
            vert.clear();
            for (int j = 0; j < sectorCount; j++)
            {
                vert.push_back(
                    sphereCenter + coordinate3f(
                        radius * sincos[j].x * sincosz[i].x,
                        radius + radius * sincos[j].y * sincosz[i].y,
                        radius * sincos[j].z * sincosz[i].x
                    )
                );
            }
            
            vertices.push_back(vert);
        }
        if(hemisphere)
            vertices.push_back(temp1);

        vector<plane> temp;
        for (int i = 0; i < vertices.size() - 1; i++)
        {
            temp=genPlane(vertices[i], vertices[int(i + 1)],baseCenter,color);
            planes.insert(planes.end(), temp.begin(), temp.end());
        }
    }
public:
    vector<plane> planes;
    GLfloat radius,height;
    int stackCount, sectorCount,n;
    coordinate3f baseCenter, topCenter,sphereCenter,color;
    bool hemisphere;
    Sphere(int _radius=0,int _stackCount=1, int _sectorCount=0, coordinate3f _baseCenter=coordinate3f(),bool _hemisphere=0,coordinate3f _color=coordinate3f(1,1,1))
    {
        hemisphere = _hemisphere;
        color = _color;
        radius = _radius;
        stackCount = _stackCount+1;
        sectorCount = _sectorCount;

        baseCenter = _baseCenter;

        topCenter = baseCenter;
        topCenter.y += hemisphere?radius:2*radius;
        
        sphereCenter = !hemisphere?baseCenter:coordinate3f(baseCenter.x,baseCenter.y-radius,baseCenter.z);
        n = hemisphere ? 2 : 1;
        height = (topCenter - baseCenter).y;
        unitCircle();
        build();
    }
};