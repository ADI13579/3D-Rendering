//#pragma once
//#include"Basic.h"
//#include<algorithm>
//float Ia = 0.4;
//float Ka = 0.8;
//float Ip = 1;
//float kd = 0.7;
//coordinate3f pointlight(0, 0, 0);
//coordinate3f camera(0, 0, 0);
//class plane
//{
//public:
//    plane(coordinate3f a = coordinate3f(), coordinate3f b = coordinate3f(), coordinate3f c = coordinate3f(), coordinate3f _ref = coordinate3f(), coordinate3f _color = coordinate3f(255,255,255))
//    {
//        ref = _ref;
//        color = coordinate3f(_color.x/255,_color.y/255,_color.z/255);
//        
//        vertex = { a, b,c };
//        sort();
//        getnormal();
//    }
//
//    vector<coordinate3f> vertex;
//    float I[3] = {0};//stores the intensity at each vertex
//    coordinate3f color;
//    coordinate3f ref;//the normal calculated are in opposite direction from this point
//    coordinate3f centroid;
//    coordinate3f normal;//this has the normal for that orginates from centroid
//    coordinate3f vertexNormal[3];//this stores the normal originating from each vertex
// 
//    //sort the vertices on the basis of y
//    void sort()
//    {
//        for (int i = 0; i < 3; i++)
//            for (int j = 0; j < 3 - 1; j++)
//                if (vertex[j].y > vertex[j + 1].y)
//                {
//                    swap(vertex[j], vertex[j + 1]);
//                    swap(I[j], I[j + 1]);
//                }
//    }
//    
//    //Calculate the intensities at each vertex
//    void vertexIntensities()
//    {
//        for (int i = 0; i < 3; i++)
//        {
//            I[i] = Ia*Ka + Ip * kd * (!normal^!(vertex[i]- pointlight));
//            I[i] = I[i] > 1 ? 1 : I[i];
//        }
//    }
//
//    //calculate normal orginateing to opposite direction from ref point
//    void getnormal()
//    {
//        centroid = (vertex[0] + vertex[1] + vertex[2]) / 3;
//        normal = (centroid - vertex[1]) * (centroid - vertex[2]);
//        if (((centroid - ref) ^ normal) > 0)
//            normal = coordinate3f() - normal;
//
//        for (int i = 0; i < 3; i++)
//        {
//            vertexNormal[i] = (vertex[i] - vertex[(i + 1) % 3]) * (vertex[i] - vertex[(i + 2) % 3]);
//            if (((vertex[i] - ref) ^ vertexNormal[i]) > 0)
//            {
//                vertexNormal[i] = coordinate3f() - vertexNormal[i];
//            }
//            vertexNormal[i] = !vertexNormal[i];
//        }
//        vertexIntensities();
//    }
//
//    plane scale(coordinate3f scalefactor)
//    {
//        plane t = *this;
//        for (int i = 0; i < t.vertex.size(); i++)
//            t.vertex[i] = t.vertex[i].scaling(scalefactor.x, scalefactor.y, scalefactor.z);
//        t.ref = t.ref.scaling(scalefactor.x, scalefactor.y, scalefactor.z);
//        t.getnormal();
//        return t;
//    }
//
//    plane translate(coordinate3f shiftVector)
//    {
//        plane t = *this;
//        for (int i = 0; i < t.vertex.size(); i++)
//            t.vertex[i] = t.vertex[i] + shiftVector;
//        t.ref = t.ref + shiftVector;
//        t.sort();
//        t.getnormal();
//        return t;
//    }
//
//   
//
//    plane rotate(GLfloat alpha, bool _x, bool _y, bool _z, coordinate3f pivot = coordinate3f())
//    {
//        plane temp = *this;
//        for (int i = 0; i < vertex.size(); i++)
//        {
//            temp.vertex[i] = temp.vertex[i].rotation(alpha, _x, _y, _z, pivot);
//        }
//        temp.ref = temp.ref.rotation(alpha, _x, _y, _z, pivot);
//        temp.sort();
//        temp.getnormal();
//        return temp;
//    }
//
//    //Gets the y of intersect point return INT_MAX if the line is horizontal 
//    int GetIntersectPoint(coordinate2i a, coordinate2i b, int x)
//    {
//        if (a.x == b.x)
//            return INT_MAX;
//
//        float m = (b.y - a.y) / float((b.x - a.x));
//        float c = (a.y - m * a.x);
//        int y = m * x + c;
//
//        return y;
//    }
//    
//    //Gets the x of intersect point return INT_MAX if the line is vertical 
//    int newGetIntersectPoint(coordinate2i a, coordinate2i b, int y)
//    {
//        if (a.x == b.x)
//            return a.x;
//
//        if (a.y == b.y)
//            return INT_MAX;
//
//        float m = (b.y - a.y) / float((b.x - a.x));
//        float c = (a.y - m * a.x);
//        int x = (y-c)/m;
//
//        return x;
//    }
//
//    //RASTERIZING PART
//    void newdraw()
//    {
//
//
//        //make a copy of vertex from to t and this is processed in rasterizing
//        vector<coordinate2i> t = {
//                                        coordinate2i(vertex[0].x,vertex[0].y),
//                                        coordinate2i(vertex[1].x,vertex[1].y),
//                                        coordinate2i(vertex[2].x,vertex[2].y),
//        };
//
//        if (t[0] == t[1] || t[1] == t[2] || t[0] == t[2])
//            return;
//        if (t[0].x == t[1].x && t[1].x == t[2].x)
//            return;
//
//        if (t[0].y == t[1].y && t[1].y == t[2].y)
//            return;
//
//        //RASTERIZE enables to use self written rasterizing on disabling OPENGL primitives are used
//        if (RASTERIZE)
//        {
//            coordinate2i temp;
//            vector<int> point;
//            
//            float I4, I5, Ip;
//            for(int y = t[0].y; y<t[1].y; y++)
//            {
//                point = {
//                                    newGetIntersectPoint(t[0], t[1], y),
//                                    newGetIntersectPoint(t[0], t[2], y),
//                };
//                
//                if (point[0] == INT_MAX)
//                    point[0] = t[1].x;
//
//                if (point[0] > point[1])
//                    swap(point[0], point[1]);
//                
//                coordinate2i p4(point[0],y);
//                coordinate2i p5(point[1],y);
//                temp.y = y;
//        
//                //1->1
//                //2->0
//                //3->2
//
//                if (t[0].y != t[1].y)
//                    I4 = ((y - t[0].y) * I[1] + (t[1].y - y) * I[0]) / (t[1].y - t[0].y);
//                else
//                    I4 =  1;
//        
//                //1->1
//                //2->0
//                //3->2
//
//                if (t[0].y != t[2].y)
//                    I5 = ((y - t[2].y) * I[0] + (t[0].y - y) * I[2]) / (t[0].y - t[2].y);
//                else
//                    I5 =  1;
//        
//
//                            
//                for (int x = point[0]; x <=point[1]; x++)
//                {
//                    //1->1
//                    //2->0
//                    //3->2
//
//                    temp.x = x;
//                    Ip = I4;
//                    if (p4.x != p5.x)
//                        Ip = ((x - p5.x) * I4 + (p4.x - x) * I5) / (p4.x - p5.x);
//                    putpixel(temp, color%Ip);
//                }
//            }   
//            
//            for (int y = t[1].y; y <= t[2].y; y++)
//            {
//
//                point = {
//                              newGetIntersectPoint(t[1], t[2], y),
//                              newGetIntersectPoint(t[0], t[2], y),
//                };
//
//                if (point[0] == INT_MAX)
//                    point[0] = t[1].x;
//
//                if (point[0] > point[1])
//                    swap(point[0], point[1]);
//
//
//                coordinate2i p4(point[0], y);
//                coordinate2i p5(point[1], y);
//                temp.y = y;
//
//                if (t[0].y != t[1].y)
//                    I4 = (I[1] - I[0]) / (t[1].y - t[0].y) * (y - t[0].y) + I[0];
//                else
//                    I4 = I[0];
//
//                if (t[1].y != t[2].y)
//                    I5 = (I[2] - I[1]) / (t[2].y - t[1].y) * (y - t[1].y) + I[1];
//                else
//                    I5 = I[1];
//
//                for (int x = point[0]; x <= point[1]; x++)
//                {
//                    temp.x = x;
//                    Ip = I4;
//                    if (p4.x != p5.x)
//                        Ip = (I5 - I4) / (p5.x - p4.x) * (x - p4.x) + I4;
//                    
//                    putpixel(temp, color % Ip);
//                }
//            }
//
//            if (Mesh)
//            {
//                Bresenham_Line(t[0], t[1], coordinate3f(0, 0, 0));
//                Bresenham_Line(t[0], t[2], coordinate3f(0, 0, 0));
//                Bresenham_Line(t[1], t[2], coordinate3f(0, 0, 0));
//            }
//        }
//        else
//        {
//            glVertexPointer(2, GL_INT, 0, &t[0]);
//            glColor3d(color.x, color.y, color.z);
//            glDrawArrays(GL_TRIANGLES, 0, 3);
//            if (Mesh)
//            {
//                vector<coordinate2i> m = {
//                                         t[0],
//                                         t[1],
//                                         t[1],
//                                         t[2],
//                                         t[2],
//                                         t[0],
//                };
//
//                glVertexPointer(2, GL_INT, 0, &m[0]);
//                glColor3d(0, 0, 0);
//                glLineWidth(1);
//                glDrawArrays(GL_LINES, 0, 6);
//            }
//        }
//    }
//       
//    void print()
//    {
//        cout << "===========================" << endl;
//        for (auto i : vertex)
//            i.print();
//    }
//};
//
//vector<plane> genPlane(vector<coordinate3f> vertices1, vector<coordinate3f> vertices2, coordinate3f ref, coordinate3f color = coordinate3f(255, 255, 255))
//{
//    vector<plane> planes;
//    int size = vertices1.size();
//
//    plane tplane;
//    for (int i = 0; i < size; i++)
//    {
//        tplane = plane(
//            vertices1[i % size],
//            vertices1[(i + 1) % size],
//            vertices2[i % size],
//            ref,
//            color
//        );
//        planes.push_back(tplane);
//    }
//
//    for (int i = 0; i < size; i++)
//    {
//        tplane = plane(
//            vertices2[i % size],
//            vertices2[(i + 1) % size],
//            vertices1[(i + 1) % size],
//            ref,
//            color
//        );
//        planes.push_back(tplane);
//    }
//    return planes;
//};
//
////Back face cull is implemented but camera direction is always to the centroid of the triangle
////So camera needed 
//vector<plane> backfacecull(vector<plane> planes)
//{
//    vector<plane> selected;
//    for (auto i : planes)
//    {
//        if ((i.normal ^ (camera - i.centroid)) < 0)
//        {
//            selected.push_back(i);
//        }
//    }
//    return selected;
//}
//
//merge sort is implemented to sort the output after backface culling
//First the plane  that lies far away is drawn first and then the near plane that lies is drawn over that palne
//this is done by sorting on the basis of z
//void merge(vector<plane>& left, vector<plane>& right, vector<plane>& bars)
//{
//    int nL = left.size();
//    int nR = right.size();
//    int i = 0, j = 0, k = 0;
//
//    while (j < nL && k < nR)
//    {
//        if (left[j].centroid.z < right[k].centroid.z) {
//            bars[i] = left[j];
//            j++;
//        }
//        else {
//            bars[i] = right[k];
//            k++;
//        }
//        i++;
//    }
//    while (j < nL) {
//        bars[i] = left[j];
//        j++; i++;
//    }
//    while (k < nR) {
//        bars[i] = right[k];
//        k++; i++;
//    }
//}
//
//
//void sort(vector<plane>& bar) {
//    if (bar.size() <= 1) return;
//
//    int mid = bar.size() / 2;
//    vector<plane> left;
//    vector<plane> right;
//
//    for (size_t j = 0; j < mid; j++)
//        left.push_back(bar[j]);
//    for (size_t j = 0; j < (bar.size()) - mid; j++)
//        right.push_back(bar[mid + j]);
//
//    sort(left);
//    sort(right);
//    merge(left, right, bar);
//}
//
//
//void world()
//{
//    cout << "Hello World"<<endl;
//}
//
//
//void threadSort(vector<plane> &planes)
//{
//    vector<plane> left,right;
//
//    left.insert(left.end(), planes.begin(), planes.begin() + planes.size() / 2);
//    right.insert(right.end(), planes.begin() + planes.size() / 2, planes.end());
//    
//    
//    cout << "Is kuskjsahhfuas";
//    merge(left, right,planes);
//}
//
////draws all triangle 
//void draw(vector<plane> planes)
//{
//    vector<plane> selected = backfacecull(planes);
//    sort(selected);
//    cout<<"Planes selected to be drawn" << selected.size() << endl;
//    for (auto i : selected)
//        i.newdraw();
//}