#pragma once
#include"plane_t.h"
#include"Basic.h"
#include<algorithm>
#include"Shader.h"

void plane_t::diffuseIntensities(coordinate3f pointlight)
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
       I[i] = kd * 0.3;

        float a = (!vn[i] ^ !(pointlight - v[i]));
        if (a > 0) I[i] = I[i] + kd * a;

    }
}void plane_t::specularIntensities(coordinate3f camera)
{
    /*
         H = unitvec(L + V)
                    L=point to light
                    V=point to camera;
        I = Idiff + Ispec
            = ka Ia + kd Il(N · L) + ks Il(N · H)^ns
        
        V is the unit vector pointing to the viewer from the selected
        surface position

        Because V and R are unit vectors in the viewing and specular-reflection directions

   */
    //Ia and Id is taken as 1 for now but needs to be a 3coordinate vector
    for (int i = 0; i < 3; i++)
    {
        coordinate3f L = !(v[i] - camera);
        float a = !(v[i]-mycamera.Position)^!(centroidNormal* 2 * (vn[i] ^ L) -L);
        if (a > 0) I[i] = I[i] + ks * (pow(a, Ns));
        I[i].x = I[i].x > 1 ? 1 : I[i].x;
        I[i].y = I[i].y > 1 ? 1 : I[i].y;
        I[i].z = I[i].z > 1 ? 1 : I[i].z;
    }

}


void plane_t::print()
{
    material::print();
    simpleplane::print();
}

float plane_t::GetIntersectPoint(coordinate2i a, coordinate2i b, int y)
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
//RASTERIZING PART
void plane_t::draw(bool MESH,std::vector<std::vector<int>> &Zbuffer, std::vector<std::vector<coordinate3f>> &pixelbuffer)
{
    //this part seems to be efficient if breshenham algorithm is used 
    sort();//this must be done befor calculating div
    //this is a common denominator for W0 and W1 expression of the barycentric interpolation method so calculated outside
    float div = (v[1].y - v[2].y) * (v[0].x - v[2].x) + (v[2].x - v[1].x) * (v[0].y - v[2].y);
    if (div == 0)
        return;

    coordinate3f A = (v[0] - v[1]) * (v[0] - v[2])*1000;
    float D = A.x * v[0].x + A.y * v[0].y + A.z * v[0].z;

    std::vector<coordinate2f> t = {
                                    coordinate2f(v[0].x,v[0].y),
                                    coordinate2f(v[1].x,v[1].y),
                                    coordinate2f(v[2].x,v[2].y),
    };
    

    for (int y = (t[0].y+0.5); y <= (t[2].y+0.5); y++)
    {

        if (y < 0)
            y = 0;
        else if (y > SCREEN_HEIGHT)
            break;


        coordinate3f temp(0, y);
        std::vector<float> point = {
                     GetIntersectPoint(t[0], t[1], y),
                     GetIntersectPoint(t[1], t[2], y),
                     GetIntersectPoint(t[2], t[0], y),
        };

        std::sort(point.begin(), point.end());
        if (point[point.size() - 1] == INT_MAX)
            point.pop_back();
        
        if (point.size() > 2)
        {
            point[1] = point[2];
            point.pop_back();
        }

        int X[]= { point[0],point[1] };
        X[0] = X[0] < 0 ? 0 : X[0];
        X[1] = X[1] > SCREEN_WIDTH ? SCREEN_WIDTH: X[1];

        for (int x = X[0]; x <= X[1]; x++)
        {
            temp.x = x;
            
            temp.z = -((D - x * A.x - y * A.y) / A.z);
            if (Zbuffer[x][y] <= temp.z + 2 || (pixelbuffer[x][y] == sky))
            {
                Zbuffer[x][y] = temp.z;
                float W0 = ((t[1].y - t[2].y) * (x - t[2].x) + (t[2].x - t[1].x) * (y - t[2].y)) / div;
                float W1 = ((t[2].y - t[0].y) * (x - t[2].x) + (t[0].x - t[2].x) * (y - t[2].y)) / div;
                float W2 = 1.0 - W0 - W1;
                coordinate3f color(I[0] * W0 + I[1] * W1 + I[2] * W2);
                putpixel(temp, color, pixelbuffer);
            }
        }

        if (MESH)
        {
               putpixel(coordinate3f(point[0], y), BLACK, pixelbuffer);
               putpixel(coordinate3f(point[1], y), BLACK, pixelbuffer);
        }
    }
    
    //since scan line already computes all points lying in edges Brshenham is not required 
    //for lines that are not horizontal
    if (MESH)
    {
        std::vector<coordinate3f> point;
        if (v[0].y == v[1].y)
            point = { v[0],v[1] };
        else if (v[1].y == v[2].y)
            point = { v[1],v[2] };

        if (point.size() > 1)
        {
            if (point[0].y > 0 && point[1].y < SCREEN_HEIGHT)
            {
                return;
            }

            int X[] = { point[0].x,point[1].x };
            if (X[0] > X[1])
                std::swap(X[0], X[1]);
            
            if (X[0] > SCREEN_WIDTH || X[1] < 0)
                return;
            
            for (int x = X[0]; x <=X[1]; x++)
            {
                if (x < 0)
                    x = 0;
                if (x > SCREEN_WIDTH)
                    x = SCREEN_WIDTH;
                pixelbuffer[x][int(point[0].y)] = BLACK;
            }
        }
    }
    if (MESH)
    {
        Bresenham_Line(t[0], t[1], coordinate3f(0, 1, 1),pixelbuffer);
        Bresenham_Line(t[0], t[2], coordinate3f(0, 1, 1),pixelbuffer);
        Bresenham_Line(t[1], t[2], coordinate3f(0, 1, 1),pixelbuffer);
    }
}
