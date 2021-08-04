#pragma once
#include"simpleplane.h"
#include"material.h"

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
        vertexnormal = _p.vertexnormal;
    }

    //Gets the x of intersect point return INT_MAX if the line is vertical 
    //code in rasterize.h
    float GetIntersectPoint(coordinate2i a, coordinate2i b, int y);
    void draw(bool);
    void calculateIntensities();
    void print();
    void calculateCentroid()
    {
        centroid = (v[0] + v[1] + v[2])/3;
    };

};
//merge sort is implemented to sort the output after backface culling
//First the plane  that lies far away is drawn firstand then the near plane that lies is drawn over that palne
//this is done by sorting on the basis of z
