#pragma once
#include"plane_t.h"
#include"Basic.h"

int plane_t::GetIntersectPoint(coordinate2i a, coordinate2i b, int y)
{
    if (a.x == b.x)
        return a.x;

    if (a.y == b.y)
        return INT_MAX;

    float m = (b.y - a.y) / float((b.x - a.x));
    float c = (a.y - m * a.x);
    int x = (y - c) / m;

    return x;
}

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
        I[i] = kd;
 
        float a = (!vn[i] ^ !(pointlight - v[i]));
        if(a>0) I[i] =I[i]+ kd*a;

        a = !(v[i] * 2 - pointlight - camera) ^ !vn[i];
        if (a > 0) I[i] = ks * (pow(a, Ns));

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
        for (int y = t[0].y; y < t[1].y; y++)
        {
            point = {
                                GetIntersectPoint(t[0], t[1], y),
                                GetIntersectPoint(t[0], t[2], y),
            };

            if (point[0] == INT_MAX)
                point[0] = t[1].x;

            if (point[0] > point[1])
                std::swap(point[0], point[1]);
            

            //2->0
            //1->1
            //3->2

            //I4 and I5 can be negative giving problems
            if (t[1].y != t[0].y)
                I4 = I[0] + (I[1].y-I[0].y)*(y - v[0].y) / (v[1].y - v[0].y);
            
            if(t[0].y!=t[2].y)
                I5 = I[0] + (I[2].y - I[0].y) * (y - v[0].y) / (v[2].y - v[0].y);
            
            
            coordinate2i p4(point[0], y);
            coordinate2i p5(point[1], y);
            temp.y = y;
            
            for(int x = point[0]; x <= point[1]; x++)
            {
                temp.x = x;
                //color = I4+(I5-I4)*(x-point[0])/(point[1]-point[0]);
                color = kd;
                if(color.x < 0) color.x=0;
                if(color.y < 0) color.y=0;
                if(color.z < 0) color.z=0;
                
                putpixel(temp, color);
            }
        }

        for (int y = t[1].y; y <= t[2].y; y++)
        {

            point = {
                          GetIntersectPoint(t[1], t[2], y),
                          GetIntersectPoint(t[0], t[2], y),
            };

            if (point[0] == INT_MAX)
                point[0] = t[1].x;

            if (point[0] > point[1])
                std::swap(point[0], point[1]);

            //2->0
            //1->1
            //3->2


            if (t[1].y != t[0].y)
                I4 = I[0] + (I[1].y - I[0].y) * (y - v[0].y) / (v[1].y - v[0].y);
            if (t[0].y != t[2].y)
                I5 = I[0] + (I[2].y - I[0].y) * (y - v[0].y) / (v[2].y - v[0].y);

            coordinate2i p4(point[0], y);
            coordinate2i p5(point[1], y);
            temp.y = y;
            for (int x = point[0]; x <= point[1]; x++)
            {
                temp.x = x;
                color = kd;
                //color = I4 + (I5 - I4) * (x - point[0]) / (point[1] - point[0]);
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