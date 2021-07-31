#pragma once
#include"coordinate.h"
#include<vector>
class simpleplane
{
public:
    simpleplane()
    {
        v.resize(3);
        vt.resize(3);
        vn.resize(3);
        vertexnormal.resize(3);
    }
    //To be taken from the .obj file
    //=================================
    std::vector<coordinate3f> v;//vertices of plane
    std::vector<coordinate2f> vt;//texture mapped to the above vertices if any
    std::vector<coordinate3f> vn;//avg unit normal of the vertices above
    //======================================
    //to be calculated
    //=====================================
    std::vector<coordinate3f> vertexnormal;
    coordinate3f centroid;
    coordinate3f centroidNormal;
    //=====================================
    void makeCalculations();
    void sort();
    void scale(float x, float y, float z);
    void translate(coordinate3f shiftVector);
    void calculateNormal();
    void print();
};