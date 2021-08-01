#pragma once
#include"plane_t.h"
#include"Basic.h"
#include<algorithm>.h
void debug();


void plane_t::calculateIntensities()
{
    /*
         H = unitvec(L + V)
                    L=point to light
                    V=point to camera;
        I = Idiff + Ispec
            = ka Ia + kd Il(N · L) + ks Il(N · H)^ns
    */
    //Ia and Id is taken as 1 for now but needs to be a 3coordinate vector
    for (int i = 0; i < 3; i++)
    {
        I[i] = kd*0.3;
 
        float a = (!vn[i] ^ !(pointlight - v[i]));
        if(a>0) I[i] =I[i]+ kd*a;

        a = !(v[i] * 2 - pointlight - camera) ^ !vn[i];
        if (a > 0) I[i] =I[i]+ks * (pow(a, Ns));

        I[i].x = I[i].x> 1 ? 1 : I[i].x;
        I[i].y = I[i].y> 1 ? 1 : I[i].y;
        I[i].z = I[i].z> 1 ? 1 : I[i].z;    
    }
}

void plane_t::print()
{
    material::print();
    simpleplane::print();
}


int plane_t::GetIntersectPoint(coordinate2i a, coordinate2i b, int y)
{
    if (a.y > b.y)
        std::swap(a, b);

    //check if there exists a intersection point
    if (y<a.y || y>b.y)
        return INT_MAX;

    //if a.y==b.y means that the x value would just be many
    //but return the largest
    if (a.y == b.y)
    {
        int c = (a.x > b.x) ? a.x : b.x;
        return c;
    }

    //if the x of two points is same then (x,y) is the 
    //intersection point hence return x itself

    if (a.x == b.x)
        return a.x;

    float m = (b.y - a.y) / float((b.x - a.x));
    float c = (a.y - m * a.x);
    int x = (y - c) / m;
    return x;
}
//RASTERIZING PART
void plane_t::draw()
{
    //back face culling
    /*if (((camera-centroid)^centroidNormal)<0)
        return;*/


    //make a copy of vertex from to t and this is processed in rasterizing
    std::vector<coordinate2i> t = {
                                    coordinate2i(v[0].x,v[0].y),
                                    coordinate2i(v[1].x,v[1].y),
                                    coordinate2i(v[2].x,v[2].y),
    };

    if (t[0] == t[1] || t[1] == t[2] || t[0] == t[2])
        return;
    if (t[0].x == t[1].x && t[1].x == t[2].x)
        return;

    if (t[0].y == t[1].y && t[1].y == t[2].y)
        return;

    bool RASTERIZE = 1;
    bool MESH = 0;
    //RASTERIZE enables to use self written rasterizing on disabling OPENGL primitives are used
  
    if (RASTERIZE)
    {
        calculateIntensities();
        coordinate3f color;
        coordinate2i temp;
        std::vector<int> point;
        coordinate3f I4, I5, Ip;

        for (int y = t[0].y; y <=t[1].y; y++)
        {
            I4 = I[0];
            if (v[1].y != v[0].y)
            {
                coordinate3f colorT = (I[1] - I[0]) * ((y - v[0].y) / (v[1].y - v[0].y));
                I4 = I4 + colorT;
            }


            I5 = I[0];
            if (v[0].y != v[2].y)
            {
                coordinate3f colorT = (I[2] - I[0]) * (y - v[0].y) / (v[2].y - v[0].y);
                I5 = I[0] + colorT;
            }

            point = {
                         GetIntersectPoint(t[0], t[1], y),
                         GetIntersectPoint(t[0], t[2], y),
            };

            if (point[0] > point[1])
            {
                std::swap(point[0], point[1]);
                std::swap(I4, I5);
            }

            coordinate2i p4(point[0], y);
            coordinate2i p5(point[1], y);
            temp.y = y;

            for (int x = point[0]; x <= point[1]; x++)
            {
                temp.x = x;
                color = I4;
                if (point[0] != point[1])
                {
                    coordinate3f colorT = (I5 - I4) * (x - point[0]) / (point[1] - point[0]);
                    color = I4 + colorT;
                }
                putpixel(temp, color);
            }
        }
        for (int y = t[1].y; y <=t[2].y; y++)
        {
            I4 = I[1];
            if (v[1].y != v[2].y)
            {
                coordinate3f colorT = (I[2] - I[1]) * ((y - v[1].y) / (v[2].y - v[1].y));
                I4 = I[1] + colorT;
            }


            I5 = I[0];
            if (v[0].y != v[2].y)
            {
                coordinate3f colorT = (I[2] - I[0]) * (y - v[0].y) / (v[2].y - v[0].y);
                I5 = I5 + colorT;
            }

            point = {
                         GetIntersectPoint(t[1], t[2], y),
                         GetIntersectPoint(t[0], t[2], y),
            };
            if (point[0] > point[1])
            {
                std::swap(point[0], point[1]);
                std::swap(I4, I5);
            }
            coordinate2i p4(point[0], y);
            coordinate2i p5(point[1], y);
            temp.y = y;

            
            for (int x = point[0]; x <=point[1]; x++)
            {
                temp.x = x;
                color = I4;
                if (point[0] != point[1])
                {
                    coordinate3f colorT = (I5 - I4) * (x - point[0]) / (point[1] - point[0]);
                    color = I4 + colorT;
                }
                putpixel(temp, color);
            }
        }

        if (MESH)
        {
            Bresenham_Line(t[0], t[1], coordinate3f(0, 1, 1));
            Bresenham_Line(t[0], t[2], coordinate3f(0, 1, 1));
            Bresenham_Line(t[1], t[2], coordinate3f(0, 1, 1));
        }
    }
    else
    {
        glVertexPointer(2, GL_INT, 0, &t[0]);
        glColor3d(kd.x, kd.y, kd.z);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        if (MESH)
        {
            std::vector<coordinate2i> m = {
                                     t[0],
                                     t[1],
                                     t[1],
                                     t[2],
                                     t[2],
                                     t[0],
            };

            glVertexPointer(2, GL_INT, 0, &m[0]);
            glColor3d(0, 0, 0);
            glLineWidth(1);
            glDrawArrays(GL_LINES, 0, 6);
        }
    }
}

void debug()
{
    int a = 1;
    std::cout << "what is wrong";
};
