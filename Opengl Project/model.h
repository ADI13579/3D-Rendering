#include"sphere.h"
#include"Object.h"
#include"Clock.h"
#include<chrono>

void mergeSort(vector<plane>& left, vector<plane>& right, vector<plane>& bars)
{
    int nL = left.size();
    int nR = right.size();
    int i = 0, j = 0, k = 0;

    while (j < nL && k < nR)
    {
        if (left[j].vertex[0].z < right[k].vertex[0].z) {
            bars[i] = left[j];
            j++;
        }
        else {
            bars[i] = right[k];
            k++;
        }
        i++;
    }
    while (j < nL) {
        bars[i] = left[j];
        j++; i++;
    }
    while (k < nR) {
        bars[i] = right[k];
        k++; i++;
    }
}


void sort(vector<plane>& bar) {
    if (bar.size() <= 1) return;

    int mid = bar.size() / 2;
    vector<plane> left;
    vector<plane> right;

    for (size_t j = 0; j < mid; j++)
        left.push_back(bar[j]);
    for (size_t j = 0; j < (bar.size()) - mid; j++)
        right.push_back(bar[mid + j]);

    sort(left);
    sort(right);
    mergeSort(left, right, bar);
}


class model:public Draw
{
private:
    void createmodel()
    {        

        obj1[0] =new Object(radius, radius, height, 5, 8,coordinate3f());
        obj1[1] =new Object(obj1[0]->topRadius * 0.84, obj1[0]->baseRadius * 0.82, obj1[0]->height, obj1[0]->stackCount, obj1[0]->sectorCount, obj1[0]->topCenter);
        obj1[2] =new Object(obj1[1]->topRadius * 0.84, obj1[1]->baseRadius * 0.82, obj1[1]->height, obj1[1]->stackCount, obj1[1]->sectorCount, obj1[1]->topCenter);
                 
        obj1[3] =new Object(obj1[2]->topRadius * 0.84, obj1[2]->baseRadius * 0.82, obj1[2]->height * 20, obj1[2]->stackCount, obj1[2]->sectorCount, obj1[2]->topCenter);
        obj1[4] =new Object(obj1[3]->topRadius, obj1[0]->topRadius, obj1[1]->height*3 , obj1[1]->stackCount, obj1[1]->sectorCount, obj1[3]->topCenter);
                 
        obj1[5] =new Object(obj1[4]->topRadius, obj1[4]->topRadius, obj1[1]->height, obj1[1]->stackCount, obj1[1]->sectorCount, obj1[4]->topCenter);
        obj1[6] =new Object(obj1[1]->topRadius, obj1[1]->baseRadius, obj1[1]->height * 14, obj1[1]->stackCount, 4, obj1[5]->topCenter);
        
        obj2 = new Sphere(obj1[6]->topRadius, 10, 8, obj1[6]->topCenter, 1);
        obj3 = new Object(obj2->radius * 0.2, 0, obj2->radius, 1, obj2->sectorCount, coordinate3f(obj2->topCenter.x, obj2->topCenter.y * 0.998, obj2->topCenter.z));
        obj4 = new Sphere(obj3->baseRadius, 3, obj2->sectorCount, coordinate3f(obj3->topCenter.x, obj3->topCenter.y - obj3->height * 0.6, obj3->topCenter.z), 0);
    

        baseCenter = obj1[0]->baseCenter;
        watch = new Clock(obj1[6]->topRadius / sqrt(2), coordinate3f(obj1[6]->baseCenter.x, (obj1[6]->baseCenter.y + obj1[6]->topCenter.y) / 2, obj1[6]->baseCenter.z), obj1[6]->baseRadius / sqrt(1.8), &baseCenter);

        topCenter = obj4->topCenter;
        modelCenter = baseCenter;

    }
public:
    vector<plane> planes;
    coordinate3f modelCenter, baseCenter, topCenter;
    Object *obj1[7];
    Sphere *obj2;
    Object *obj3;
    Sphere *obj4;
    GLfloat radius, height, modelHeight;
    Clock *watch;
    model(int _radius = 0)
    {
        planes.clear();
        radius = _radius;
        height = radius / 7;
        createmodel();
        
        for (int i = 0; i < 7; i++)
        {
            planes.insert(planes.end(), obj1[i]->planes.begin(), obj1[i]->planes.end());
            delete(obj1[i]);
        }
        
        planes.insert(planes.end(), obj2->planes.begin(), obj2->planes.end());
        planes.insert(planes.end(), obj3->planes.begin(), obj3->planes.end());
        planes.insert(planes.end(), obj4->planes.begin(), obj4->planes.end());
        planes.insert(planes.end(), watch->planes.begin(), watch->planes.end());
        
        delete(obj2);
        delete(obj3);
        delete(obj4);
    }

    
    
    void translate(coordinate3f shiftVector)
    {
        for (int i = 0; i < planes.size(); i++)
        {
            planes[i] = planes[i].translate(shiftVector);
        }
        baseCenter = baseCenter + shiftVector;
        watch->clockCenter = watch->clockCenter + shiftVector;    
    }
    
    void Draw()
    {
     
       watch->buildhand();

       coordinate3f camera=worldprops::camera;
       coordinate3f CameraToBase,ref, vertexTobase, normal;
       vector<plane> filteredplanes[3];
       
       vector<plane> PlanesDrawn;
       coordinate3f sum;
       
       vector<plane> test;

       for (auto i : watch->planes2)
       {
           i = i.transform();
           test.push_back(i);
       }

       for (auto i : planes)
       {
           i = i.transform();
           test.push_back(i);
           sum = sum + i.vertex[0] + i.vertex[1] + i.vertex[2];
       }

       ref = sum / (planes.size()*3);
       vector<coordinate3f> po(1, ref);
     
       //back face culling
       for(auto i : test)
       {
           
           normal = (i.vertex[0] - i.vertex[1]) * (i.vertex[0] - i.vertex[2]);
           CameraToBase = camera - ref;
           
           if (((i.vertex[0]-ref) ^ normal) < 0)
               normal = coordinate3f() - normal;

           if ((normal ^ CameraToBase) > 0)
               PlanesDrawn.push_back(i);
           else
               filteredplanes[0].push_back(i);
       }

       
    DrawPoint(po, coordinate3f(),3);

    cout << "Total Planes" << planes.size() << endl;
    cout << "Planes Discarded:" << planes.size() + watch->planes2.size() - PlanesDrawn.size() << endl;
    cout << "Camera coordianates:"; camera.print();
    cout << "Ref point:"; ref.print();
      
     glEnable(GL_DEPTH_TEST);
     glDepthMask(GL_TRUE);
       
     sort(PlanesDrawn);
    for (auto i :PlanesDrawn)
    {
         i.drawPlane();
         i.drawMesh();
    }  
       vector<coordinate3f> p = { worldprops::camera };
       DrawPoint(p,0,10);
    }
};