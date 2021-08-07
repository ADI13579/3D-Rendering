#pragma once
#include"plane_t.h"
#include"Basic.h"
#include<algorithm>
#include"Shader.h"
void putpixel(coordinate3f pixel, coordinate3f color, float alpha)
{
    if ((pixel.x < 0 || pixel.x > SCREEN_WIDTH))
        return;

    if ((pixel.y < 0 || pixel.y > SCREEN_HEIGHT))
        return;
    
 
        Zbuffer[int(pixel.x)][int(pixel.y)] = pixel.z;
        glColor3f(color.x, color.y, color.z);
        glVertexPointer(2, GL_FLOAT, 0, &pixel.x);
        glDrawArrays(GL_POINTS, 0, 1);
};

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
}void plane_t::specularIntensities()
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
        float a = !(v[i] * 2 - pointlight - mycamera.Position) ^ !vn[i];
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
void plane_t::draw(bool MESH,std::vector<std::vector<int>> &Zbuffer)
{
    sort();
    coordinate3f A = (v[0] - v[1]) * (v[0] - v[2]);
    float D = A.x * v[0].x + A.y * v[0].y + A.z * v[0].z;

    //this is a common denominator for W0 and W1 expression of the barycentric interpolation method so calculated outside
    float div = (v[1].y - v[2].y) * (v[0].x - v[2].x) + (v[2].x - v[1].x) * (v[0].y - v[2].y);
    if (div == 0)
        return;

    std::vector<coordinate2i> t = {
                                    coordinate2i(v[0].x,v[0].y),
                                    coordinate2i(v[1].x,v[1].y),
                                    coordinate2i(v[2].x,v[2].y),
    };
    
    for (int y = t[0].y; y <= t[2].y; y++)
    {
        if (y > SCREEN_HEIGHT)
            break;
        if (y < 0)
            y = 0;

        coordinate3f temp(0, y);
        std::vector<float> point = {
                     GetIntersectPoint(t[0], t[1], y),
                     GetIntersectPoint(t[1], t[2], y),
                     GetIntersectPoint(t[2], t[0], y),
        };

        std::sort(point.begin(), point.end());
        if (point[2] == INT_MAX)
            point.pop_back();

        //part for clippig points that lies outside
        //since x1 to x2 is already sorted the line doesnt lie inside if x1<0 || x2>SCREEN_WIDTH
        if (!(point[0] > SCREEN_WIDTH || point[1] < 0))
        {
            if (point[0] < 0)
                point[0] = 0;
            if (point[1] > SCREEN_WIDTH)
                point[1] = SCREEN_WIDTH;

            for (int x = point[0]; x <= point[1]; x++)
            {
                temp.x = x;
                temp.z = (D - x * A.x - y * A.y) / A.z;
                temp.z += (D - (x-1) * A.x - (y+1) * A.y) / A.z;
                temp.z /= 2;

                if (Zbuffer[x][y] < temp.z)
                {
                    Zbuffer[x][y] = temp.z;
                    float W0 = ((t[1].y - t[2].y) * (x - t[2].x) + (t[2].x - t[1].x) * (y - t[2].y)) / div;
                    float W1 = ((t[2].y - t[0].y) * (x - t[2].x) + (t[0].x - t[2].x) * (y - t[2].y)) / div;
                    float W2 = 1.0 - W0 - W1;
                    coordinate3f color(I[0] * W0 + I[1] * W1 + I[2] * W2);

                    putpixel(temp, color, d);
                }
            }
        }
    }
    if (MESH)
    {
        Bresenham_Line(t[0], t[1], coordinate3f(0, 1, 1));
        Bresenham_Line(t[0], t[2], coordinate3f(0, 1, 1));
        Bresenham_Line(t[1], t[2], coordinate3f(0, 1, 1));
    }
}
