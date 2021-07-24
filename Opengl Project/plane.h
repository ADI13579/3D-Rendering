#pragma once
#include"Basic.h"
#include<algorithm>
#include<thread>
bool terminateThread = 0;

class plane
{
public:
    vector<coordinate3f> vertex;
    coordinate3f color;
    coordinate3f ref;
    coordinate3f normal, centroid, vertexNormal[3];
    int count;
    void sort()
    {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3 - 1; j++)
                if (vertex[j].z < vertex[j + 1].z)
                    swap(vertex[j], vertex[j + 1]);
    }
    void getnormal()
    {
        centroid = (vertex[0] + vertex[1] + vertex[2]) / 3;

        normal = (centroid - vertex[1]) * (centroid - vertex[2]);

        if (((centroid - ref) ^ normal) > 0)
            normal = coordinate3f() - normal;

        for (int i = 0; i < 3; i++)
        {
            vertexNormal[i] = (vertex[i] - vertex[(i + 1) % 3]) * (vertex[i] - vertex[(i + 2) % 3]);
            if (((vertex[i] - ref) ^ vertexNormal[i]) > 0)
                vertexNormal[i] = coordinate3f() - vertexNormal[i];
        }
    }

    plane(coordinate3f a = coordinate3f(), coordinate3f b = coordinate3f(), coordinate3f c = coordinate3f(), coordinate3f _ref = coordinate3f(), coordinate3f _color = coordinate3f(1, 1, 1))
    {
        ref = _ref;
        color = _color;
        vertex = { a, b,c };

        sort();
        getnormal();
    }

    plane scale(coordinate3f scalefactor)
    {
        plane t = *this;
        for (int i = 0; i < t.vertex.size(); i++)
            t.vertex[i] = t.vertex[i].scaling(scalefactor.x, scalefactor.y, scalefactor.z);
        t.ref = t.ref.scaling(scalefactor.x, scalefactor.y, scalefactor.z);
        t.getnormal();
        return t;
    }

    plane translate(coordinate3f shiftVector)
    {
        plane t = *this;
        for (int i = 0; i < t.vertex.size(); i++)
            t.vertex[i] = t.vertex[i] + shiftVector;
        t.ref = t.ref + shiftVector;
        t.sort();
        t.getnormal();
        return t;
    }

    plane transform()
    {
        plane temp = *this;

        for (int i = 0; i < temp.vertex.size(); i++)
        {
            coordinate3f t = temp.vertex[i];
            t = t - worldprops::camera;
            t = coordinate3f(worldprops::Rot[0] ^ t, worldprops::Rot[1] ^ t, worldprops::Rot[2] ^ t);
            t = coordinate3f(worldprops::Scale[0] ^ t, worldprops::Scale[1] ^ t, worldprops::Scale[2] ^ t);
            t = t + worldprops::camera;
            temp.vertex[i] = t;
        }
        temp.sort();
        temp.getnormal();
        return temp;
    }

    plane rotate(GLfloat alpha, bool _x, bool _y, bool _z, coordinate3f pivot = coordinate3f())
    {
        plane temp = *this;
        for (int i = 0; i < vertex.size(); i++)
        {
            temp.vertex[i] = temp.vertex[i].rotation(alpha, _x, _y, _z, pivot);
        }
        temp.ref = temp.ref.rotation(alpha, _x, _y, _z, pivot);
        temp.sort();
        temp.getnormal();
        return temp;
    }

    int GetIntersectPoint(coordinate2i a, coordinate2i b, int x)
    {
        if (a.x == b.x)
            return INT_MAX;

        float m = (b.y - a.y) / float((b.x - a.x));
        float c = (a.y - m * a.x);
        int y = m * x + c;

        return y;
    }

    void draw()
    {
        vector<coordinate2i> t = {
                                        coordinate2i(vertex[0].x,vertex[0].y),
                                        coordinate2i(vertex[1].x,vertex[1].y),
                                        coordinate2i(vertex[2].x,vertex[2].y),
        };
        if (t[0] == t[1] || t[1] == t[2] || t[0] == t[2])
            return;
        if (t[0].x == t[1].x && t[1].x == t[2].x)
            return;

        if (t[0].y == t[1].y && t[1].y == t[2].y)
            return;

        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3 - 1; j++)
                if (t[j].x > t[j + 1].x)
                    swap(t[j], t[j + 1]);

        
        if (RASTERIZE)
        {
            coordinate2i temp;
            vector<int> point;
            for (int x = t[0].x; x <=t[1].x; x++)
            {
                point = {
                                                GetIntersectPoint(t[0], t[1], x),
                                                GetIntersectPoint(t[0], t[2], x),
                };
                
                if (point[0] == INT_MAX)
                    point[0] = t[1].y;

                if (point[0] > point[1])
                    swap(point[0], point[1]);
                
                temp.x = x;
                for (int y = point[0]; y <=point[1]; y++)
                {
                    temp.y = y;
                    putpixel(temp, color);
                }

            }

            for (int x = t[1].x; x <=t[2].x; x++)
            {
                point = {
                                            GetIntersectPoint(t[1], t[2], x),
                                            GetIntersectPoint(t[0], t[2], x),
                         };

                if (point[0] == INT_MAX)
                    point[0] = t[1].y;

                if (point[0] > point[1])
                    swap(point[0], point[1]);

                temp.x = x;
                for (int y = point[0]; y <=point[1]; y++)
                {
                    temp.y = y;
                    putpixel(temp, color);
                }
            }

            if (Mesh)
            {
                Bresenham_Line(t[0], t[1], coordinate3f(0, 0, 0));
                Bresenham_Line(t[0], t[2], coordinate3f(0, 0, 0));
                Bresenham_Line(t[1], t[2], coordinate3f(0, 0, 0));
            }
        }
        else
        {
            glVertexPointer(2, GL_INT, 0, &t[0]);
            glColor3d(color.x/255, color.y/255, color.z/255);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            if (Mesh)
            {
                vector<coordinate2i> m = {
                                         t[0],
                                         t[1],
                                         t[1],
                                         t[2],
                                         t[2],
                                         t[0]
                };

                glVertexPointer(2, GL_INT, 0, &m[0]);
                glColor3d(0, 0, 0);
                glLineWidth(2);
                glDrawArrays(GL_LINES, 0, 6);
            }
        }
    }

    void print()
    {
        cout << "===========================" << endl;
        for (auto i : vertex)
            i.print();
    }
};

vector<plane> genPlane(vector<coordinate3f> vertices1, vector<coordinate3f> vertices2, coordinate3f ref, coordinate3f color = coordinate3f(1, 1, 1))
{
    vector<plane> planes;
    int size = vertices1.size();

    plane tplane;
    for (int i = 0; i < size; i++)
    {
        tplane = plane(
            vertices1[i % size],
            vertices1[(i + 1) % size],
            vertices2[i % size],
            ref,
            color
        );
        planes.push_back(tplane);
    }

    for (int i = 0; i < size; i++)
    {
        tplane = plane(
            vertices2[i % size],
            vertices2[(i + 1) % size],
            vertices1[(i + 1) % size],
            ref,
            color
        );
        planes.push_back(tplane);
    }
    return planes;
};



void threadprocess(vector<plane> planes)
{
    for (auto i : planes)
        i.draw();
    terminateThread = 1;
}

vector<plane> backfacecull(vector<plane> planes)
{
    vector<plane> selected;
    for (auto i : planes)
    {
        if ((i.normal ^ (worldprops::camera - i.centroid)) < 0)
        {
            selected.push_back(i);
        }
    }
    return selected;
}

void draw(vector<plane> planes)
{
    vector<plane> selected = backfacecull(planes);
    
    
    for (auto i : selected)
        i.draw();
}