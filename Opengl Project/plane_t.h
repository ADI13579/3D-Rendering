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
    plane_t() {
        id ="";
        Ns =0;
        ka =0;
        kd =0;
        ks =0;
        ke =0;
        Ni =0;
        d = 0;

        tex = NULL;
     };
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

        tex = _m.tex;
        v = _p.v;
        vt = _p.vt;
        vn = _p.vn;

        centroid = _p.centroid;
        centroidNormal = _p.centroidNormal;
        if (tex)
            kd = coordinate3f(1, 1, 1);
    }

    //Gets the x of intersect point return INT_MAX if the line is vertical 
    //code in rasterize.h
    float GetIntersectPoint(coordinate3f a, coordinate3f b, int y);
    void draw(bool,std::vector<std::vector<float>> &Zbuffer, std::vector<std::vector<coordinate3f>> &pixels);
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

    void rotate(float alpha,coordinate3f pivot)
    {
        for (int i=0;i<3;i++)
        {
            v[i]=v[i].rotation(alpha, 0, 1, 0, pivot);
            //v[i]=v[i].rotation(alpha, 1, 0, 0, pivot);
            //v[i]=v[i].rotation(alpha, 0, 0, 1, pivot);
        }
        makeCalculations();
    }
    void attenuate(coordinate3f);
    void ambientIntensities(float Ia);
    void diffuseIntensities(coordinate3f pointlight);
    void specularIntensities(coordinate3f pointlight,coordinate3f camera);
    void print();
    void calculateCentroid()
    {
        centroid = (v[0] + v[1] + v[2])/3;
    };

    coordinate3f getcolor(coordinate3f v)
    {
        /*float intpart;
        int tx = 0.5+std::modf(v.v.texCoord.x / v.v.position.z, &intpart) * (tex->width - 1);
        int ty = 0.5+std::modf(v.v.texCoord.y / v.v.position.z, &intpart) * (tex->height - 1);
        
        return tex->imagedata[tx][ty];*/

        return coordinate3f(0.5,0.5,0.5);
    }
};
//merge sort is implemented to sort the output after backface culling
//First the plane  that lies far away is drawn firstand then the near plane that lies is drawn over that palne
//this is done by sorting on the basis of z