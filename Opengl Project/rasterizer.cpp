#pragma once
#include"plane_t.h"
#include"Basic.h"
#include<algorithm>
#include"Shader.h"

void plane_t::diffuseIntensities(coordinate3f pointlight,float p)
{
    /*
         H = unitvec(L + V)
                    L=point to light
                    V=point to camera;
        I = Idiff + Ispec
            = ka Ia + kd Il(N . L) + ks Il(N . H)^ns
    */
    //Ia and Id is taken as 1 for now but needs to be a 3coordinate vector
    for (int i = 0; i < 3; i++)
    {
        float a = (!vn[i] ^ !(pointlight - v[i]));
        if (a > 0) 
        { 
            I[i] = I[i] + kd * a*p;
            I[i].x = I[i].x > 1 ? 1 : I[i].x;
            I[i].y = I[i].y > 1 ? 1 : I[i].y;
            I[i].z = I[i].z > 1 ? 1 : I[i].z;
        }
    }
}

void plane_t::specularIntensities(coordinate3f pointlight,coordinate3f camera,float p)
{
    /*
         H = unitvec(L + V)
                    L=point to light
                    V=point to camera;
        I = Idiff + Ispec
            = ka Ia + kd Il(N . L) + ks Il(N . H)^ns

        V is the unit vector pointing to the viewer from the selected
        surface position

        Because V and R are unit vectors in the viewing and specular-reflection directions
   */
   //Ia and Id is taken as 1 for now but needs to be a 3coordinate vector
    for (int i = 0; i < 3; i++)
    {
        coordinate3f L = !(v[i] - pointlight);
        coordinate3f R = centroidNormal * (2 * (centroidNormal ^ L)) - L;
        coordinate3f V = !(v[i] - camera);

        float a = V ^ R;
        if (a >= 0) 
        {
            I[i] = I[i] + ks * (pow(a, Ns))*p;
            I[i].x = I[i].x > 1 ? 1 : I[i].x;
            I[i].y = I[i].y > 1 ? 1 : I[i].y;
            I[i].z = I[i].z > 1 ? 1 : I[i].z;
        }
    }
}

void plane_t::ambientIntensities(float Ia)
{
    for (int i = 0; i < 3; i++)
    {
        I[i] = I[i] + kd * Ia;
        I[i].x = I[i].x > 1 ? 1 : I[i].x;
        I[i].y = I[i].y > 1 ? 1 : I[i].y;
        I[i].z = I[i].z > 1 ? 1 : I[i].z;
    }
}

void plane_t::attenuate(coordinate3f pointlight)
{
    for (int i = 0; i < 3; i++)
    {
        float d = v[i].distance(pointlight)/10000;
        float k = 0.5 + 0.01 * d + 0.1 * d *d;
        I[i] = I[i] / k;
    }
}

void plane_t::print()
{
    material::print();
    simpleplane::print();
}

float plane_t::GetIntersectPoint(coordinate3f a, coordinate3f b, int y)
{
    if (a.y > b.y)
        std::swap(a, b);

    //check if there exists a intersection point
    if (y<a.y || y>b.y)
        return INT_MAX;

    //if a.y==b.y means that the line is horizontal
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
void plane_t::draw(bool WIREFRAME, std::vector<std::vector<float>>& Zbuffer, std::vector<std::vector<coordinate3f>>& pixelbuffer)
{
    if(WIREFRAME)
    {
        Bresenham_Line(v[0], v[1], coordinate3f(0, 1, 1), pixelbuffer);
        Bresenham_Line(v[0], v[2], coordinate3f(0, 1, 1), pixelbuffer);
        Bresenham_Line(v[1], v[2], coordinate3f(0, 1, 1), pixelbuffer);
    }
    else
    {
        sort();//this must be done befor calculating div
        //this is a common denominator for W0 and W1 expression of the barycentric interpolation method so calculated outside
        float div = (v[1].y - v[2].y) * (v[0].x - v[2].x) + (v[2].x - v[1].x) * (v[0].y - v[2].y);
        if (div == 0)//THIS MEANS THE TRIANGLE AREA IS 0
            return;

        for (int y = v[0].y; y <= v[2].y; y++)
        {
            if (y < 0)
                y = 0;
            else if (y > SCREEN_HEIGHT-1)
                break;

            std::vector<float> point = {
                         floor(GetIntersectPoint(v[0], v[1], y)),
                         floor(GetIntersectPoint(v[1], v[2], y)),
                         floor(GetIntersectPoint(v[2], v[0], y)),
            };

            std::sort(point.begin(), point.end());
            if (point[point.size() - 1] == INT_MAX)
                point.pop_back();
            else if (point.size() > 2)
            {
                point[1] = point[2];
                point.pop_back();
            }

            int X[] = { point[0],point[1] };
            X[0] = X[0] < 0 ? 0 : X[0];//CLIPPING IF THE LEFT OF X LIES OUTSIDE WINDOW
            X[1] = X[1] > SCREEN_WIDTH-1 ? SCREEN_WIDTH-1 : X[1];//CLIPPING IF RIGHT OF X LIES OUTSIDE WINDOW
            
            for (int x = X[0]; x <= X[1]; x++)
            {
                float W0 = ((v[1].y - v[2].y) * (x - v[2].x) + (v[2].x - v[1].x) * (y - v[2].y)) / div;
                float W1 = ((v[2].y - v[0].y) * (x - v[2].x) + (v[0].x - v[2].x) * (y - v[2].y)) / div;
                float W2 = 1.0 - W0 - W1;

                float Z = W0 / v[0].z + W1 / v[1].z + W2 / v[2].z;
                float z = 1 / Z;

                //if previous z is far away then draw the current picel (x,y)
                if (Zbuffer[y][x] > z)
                {
                    Zbuffer[y][x] = z;
                    coordinate3f color(I[0] * W0 + I[1] * W1 + I[2] * W2);
                    coordinate2f point(x, y);

                    if (!tex)
                        putpixel(point, color, pixelbuffer);
                    else
                    {
                        //Resource for texture mapping->http://archive.gamedev.net/archive/reference/articles/article331.html
                        coordinate2f te = vt[0] / v[0].z * W0 + vt[1] / v[1].z * W1 + vt[2] * 1 / v[2].z * W2;
                        te = te *z;

                        te.x = ((te.x - floor(te.x)) * (tex->width - 1));
                        te.y = ((te.y - floor(te.y)) * (tex->height - 1));


                        /*
                            for bilinear interpolation
                            Suppose that you want to interpolate on the unit square.Assume you know the values of a continuous function at the corners of the square :
                            z00 is the function value at(0, 0), the lower left corner
                            z01 is the function value at(0, 1), the upper left corner
                            z10 is the function value at(1, 0), the lower right corner
                            z11 is the function value at(1, 1), the upper right corner

                            If(x, y) is any point inside the unit square, the interpolation at that point is the following weighted average of the values at the four corners :
                            F(x, y) = z00 * (1 - x) * (1 - y) + z10 * x * (1 - y) + z01 * (1 - x) * y + z11 * x * y
                        */

                        coordinate3f col(1, 1, 1);

                        if (te.x > 0 && te.x < tex->width - 1 && te.y>0 && te.y < tex->height - 1)
                        {
                            coordinate3f P[2][2] = {
                                                    {tex->imagedata[int(te.y - 1)][int(te.x - 1)],tex->imagedata[int(te.y + 1)][int(te.x - 1)]},
                                                    {tex->imagedata[int(te.y - 1)][int(te.x + 1)],tex->imagedata[int(te.y + 1)][int(te.x + 1)]}
                            };
                            int x = te.x - floor(te.x);
                            int y = te.y - floor(te.y);
                            col = P[0][0] * (1 - x) * (1 - y) + P[1][0] * x * (1 - y) + P[0][1] * (1 - x) * y + P[1][1] * x * y;
                        }
                        else
                            col = tex->imagedata[te.y][te.x];

                        color.x *= col.x;
                        color.y *= col.y;
                        color.z *= col.z;
                        putpixel(point, color, pixelbuffer);
                    }
                }
            }
        }
    }
}