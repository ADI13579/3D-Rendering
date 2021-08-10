#pragma once
#include"simpleplane.h"
#include"material.h"
#include"Basic.h"

class plane_t :public simpleplane, public material
{
public:
    //to be calculated
    //=========================
    coordinate3f I[3];
    plane_t() {};
    plane_t(simpleplane _p, material _m)
    {
        id = _m.id;
        Ns = _m.Ns;
        ka = _m.ka;
        kd = _m.kd;
        ks = _m.ks;
        ke = _m.ke;
        Ni = _m.Ni;
        d = _m.d;

        v = _p.v;
        vt = _p.vt;
        vn = _p.vn;
        centroid = _p.centroid;
        centroidNormal = _p.centroidNormal;
        diffuseIntensities(pointlight);
    }

    //Gets the x of intersect point return INT_MAX if the line is vertical 
    //code in rasterize.h
    float GetIntersectPoint(coordinate2i a, coordinate2i b, int y);
    void draw(bool,std::vector<std::vector<int>> &Zbuffer, std::vector<std::vector<coordinate3f>> &pixels);
    void makeCalculations()
    {
        simpleplane::makeCalculations();
    }
    void sort()
    {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3 - 1; j++)
                if (v[j].y > v[j + 1].y)
                {
                    std::swap(v[j], v[j + 1]);
                    std::swap(vt[j], vt[j + 1]);
                    std::swap(vn[j], vn[j + 1]);
                    std::swap(I[j], I[j + 1]);
                }
    }

    void rotate(float alpha)
    {
        for (int i=0;i<3;i++)
        {
            v[i]=v[i].rotation(alpha, 0, 1, 0, coordinate3f(SCREEN_WIDTH/2,SCREEN_WIDTH/2,-500));
            //v[i]=v[i].rotation(alpha, 1, 0, 0, coordinate3f(SCREEN_WIDTH/2,SCREEN_WIDTH/2,-500));
            //v[i]=v[i].rotation(alpha, 0, 0, 1, coordinate3f(SCREEN_WIDTH/2,SCREEN_WIDTH/2,-500));
        }
    }
    void diffuseIntensities(coordinate3f);
    void specularIntensities();
    void print();
    void calculateCentroid()
    {
        centroid = (v[0] + v[1] + v[2])/3;
    };

};
//merge sort is implemented to sort the output after backface culling
//First the plane  that lies far away is drawn firstand then the near plane that lies is drawn over that palne
//this is done by sorting on the basis of z