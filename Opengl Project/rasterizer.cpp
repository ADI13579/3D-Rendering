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
            = ka Ia + kd Il(N . L) + ks Il(N . H)^ns
    */
    //Ia and Id is taken as 1 for now but needs to be a 3coordinate vector
    for (int i = 0; i < 3; i++)
    {
        I[i] = kd * 0.3;

        float a = (!vn[i] ^ !(pointlight - v[i]));
        if (a > 0) I[i] = I[i] + kd * a;
    }

}

void plane_t::specularIntensities(coordinate3f camera)
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
//RASTERIZING PART
void plane_t::draw(bool MESH, std::vector<std::vector<float>>& Zbuffer, std::vector<std::vector<coordinate3f>>& pixelbuffer)
{
    //this part seems to be efficient if breshenham algorithm is used 
    sort();//this must be done befor calculating div
    //this is a common denominator for W0 and W1 expression of the barycentric interpolation method so calculated outside
    float div = (v[1].y - v[2].y) * (v[0].x - v[2].x) + (v[2].x - v[1].x) * (v[0].y - v[2].y);
    if (div == 0)
        return;

    coordinate3f A = (v[0] - v[1]) * (v[0] - v[2]) * 1000;
    float D = A.x * v[0].x + A.y * v[0].y + A.z * v[0].z;

    std::vector<coordinate2f> t = {
                                    coordinate2f(v[0].x,v[0].y),
                                    coordinate2f(v[1].x,v[1].y),
                                    coordinate2f(v[2].x,v[2].y),
    };

    for (int y = int(t[0].y + 0.5); y <= int(t[2].y + 0.5); y++)
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

        int X[] = { point[0],point[1] };
        X[0] = X[0] < 0 ? 0 : X[0];
        X[1] = X[1] > SCREEN_WIDTH ? SCREEN_WIDTH : X[1];

        for (int x = int((X[0]+0.5)); x <= int(X[1]+0.5); x++)
        {
            temp.x = x;
            temp.z = -((D - x * A.x - y * A.y) / A.z);
            
            if (Zbuffer[y][x] <= (temp.z))
            {
                Zbuffer[y][x] = temp.z;
                float W0 = ((v[1].y - v[2].y) * (x - v[2].x) + (v[2].x - v[1].x) * (y - v[2].y)) / div;
                float W1 = ((v[2].y - v[0].y) * (x - v[2].x) + (v[0].x - v[2].x) * (y - v[2].y)) / div;
                float W2 = 1.0 - W0 - W1;

                coordinate3f color(I[0] * W0 + I[1] * W1 + I[2] * W2);

                if (!tex)
                    putpixel(temp, color, pixelbuffer);
                else
                {
                    //Resource for texture mapping->http://archive.gamedev.net/archive/reference/articles/article331.html

                    W0 /=v[0].z;
                    W1 /=v[1].z;
                    W2 /=v[2].z;

                   coordinate2f te = vt[0] * W0 + vt[1] * W1 + vt[2] * W2;
                   float Z=W0 + W1 + W2;
                   te = te *(1/ Z);


                    te.x = fabs((te.x - floor(te.x)) * tex->width);
                    te.y = fabs((te.y - floor(te.y)) * tex->height);


                    /*
                        for bilinear interpolation however this doesn't show much effect
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
                        coordinate3f z[2][2] = {
                                                {tex->imagedata[te.y - 1][te.x - 1],tex->imagedata[te.y + 1][te.x - 1]},
                                                {tex->imagedata[te.y - 1][te.x + 1],tex->imagedata[te.y + 1][te.x + 1]}
                        };
                        int x = te.x - floor(te.x);
                        int y = te.y - floor(te.y);
                        col = z[0][0] * (1 - x) * (1 - y) + z[1][0] * x * (1 - y) + z[0][1] * (1 - x) * y + z[1][1] * x * y;
                    }
                    else if(int(te.x)==0 && te.y<tex->height && te.y>0)
                    {
                        col = tex->imagedata[te.y][te.x];
                    }
                    else if(int(te.y)==tex->height-1 && te.x<tex->width && te.x>0)
                    {
                        col = tex->imagedata[te.y][te.x];
                    }


                    color.x *= col.x;
                    color.y *= col.y;
                    color.z *= col.z;
                    putpixel(temp, color, pixelbuffer);
                }
            }
            if (MESH)
            {
                putpixel(coordinate2f(X[0], y), coordinate3f(1, 0, 0), pixelbuffer);
                putpixel(coordinate2f(X[1], y), coordinate3f(1, 0, 0), pixelbuffer);
            }
        }
    }
    //for mesh the above code cannot draw horizontal lines
    if (MESH)
    {
        float y;
        std::vector<float> points;
        if (v[0].y == v[1].y)
        {
            y = v[0].y;
            points = { v[0].x,v[1].x };
        }
        else if (v[1].y == v[2].y)
        {
            y = v[1].y;
            points = { v[1].x,v[2].x };
        }
        if (points.size() != 0)
        {
            if (points[0] > points[1])
                std::swap(points[0], points[1]);
            for (int x = points[0]; x < points[1]; x++)
            {
                putpixel(coordinate2f(x, y), coordinate3f(1, 0, 0), pixelbuffer);
            }
        }
    }
    //since scan line method used for rasterizing already computes all points lying in edges Brshenham is not required if rasterized and mesh triangle needs to be displayed
    // however only for mesh view breshenham is effective 
      

    /*if (MESH)
    {
        Bresenham_Line(v[0], v[1], coordinate3f(0, 1, 1), pixelbuffer);
        Bresenham_Line(v[0], v[2], coordinate3f(0, 1, 1), pixelbuffer);
        Bresenham_Line(v[1], v[2], coordinate3f(0, 1, 1), pixelbuffer);
    }*/
}