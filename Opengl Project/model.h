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
        
        obj1[0] =new Object(radius, radius, height, 1, 8,coordinate3f(),coordinate3f(0,0,0));
        obj1[1] =new Object(obj1[0]->topRadius * 0.84, obj1[0]->baseRadius * 0.82, obj1[0]->height, obj1[0]->stackCount, obj1[0]->sectorCount, obj1[0]->topCenter, coordinate3f(0, 0, 0));
        obj1[2] =new Object(obj1[1]->topRadius * 0.84, obj1[1]->baseRadius * 0.82, obj1[1]->height, obj1[1]->stackCount, obj1[1]->sectorCount, obj1[1]->topCenter, coordinate3f(0, 0, 0));
                 
        obj1[3] =new Object(obj1[2]->topRadius * 0.84, obj1[2]->baseRadius * 0.82, obj1[2]->height * 20, obj1[2]->stackCount, obj1[2]->sectorCount, obj1[2]->topCenter, coordinate3f(0, 0, 0));
        obj1[4] =new Object(obj1[3]->topRadius, obj1[0]->topRadius, obj1[1]->height*3 , obj1[1]->stackCount, obj1[1]->sectorCount, obj1[3]->topCenter, coordinate3f(0, 0, 0));
                 
        obj1[5] =new Object(obj1[4]->topRadius, obj1[4]->topRadius, obj1[1]->height, obj1[1]->stackCount, obj1[1]->sectorCount, obj1[4]->topCenter, coordinate3f(0, 0, 0));
        obj1[6] =new Object(obj1[1]->topRadius, obj1[1]->baseRadius, obj1[1]->height * 14, obj1[1]->stackCount, 4, obj1[5]->topCenter, coordinate3f(0, 0, 0));
        
        obj2 = new Sphere(obj1[6]->topRadius, 10, 8, obj1[6]->topCenter, 1, coordinate3f(0, 0, 0));
        obj3 = new Object(obj2->radius * 0.2, 0, obj2->radius, 1, obj2->sectorCount, coordinate3f(obj2->topCenter.x, obj2->topCenter.y * 0.998, obj2->topCenter.z), coordinate3f(0, 0, 0));
        obj4 = new Sphere(obj3->baseRadius, 3, obj2->sectorCount, coordinate3f(obj3->topCenter.x, obj3->topCenter.y - obj3->height * 0.6, obj3->topCenter.z), 0, coordinate3f(0, 0, 0));
    

        //for (int i = 0; i < 7; i++)
        //{
        //    modelHeight+=obj1[i]->height;
        //}
        //modelHeight += obj2->height + obj3->height + obj4->height;
        baseCenter = obj1[0]->baseCenter;
        watch = new Clock(obj1[6]->topRadius / sqrt(2), coordinate3f(obj1[6]->baseCenter.x, (obj1[6]->baseCenter.y + obj1[6]->topCenter.y) / 2, obj1[6]->baseCenter.z), obj1[6]->baseRadius / sqrt(1.8), &baseCenter);

        topCenter = obj4->topCenter;
        modelCenter = baseCenter;

      
    }
public:
    vector<plane> planes,copy_planes;
    coordinate3f modelCenter, baseCenter, topCenter;
    Object *obj1[7];
    Sphere *obj2;
    Object *obj3;
    Sphere *obj4;
    GLfloat radius, height, modelHeight;
    Clock *watch;
    Sphere test;
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
        test = Sphere(40, 10, 20, baseCenter);
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

    void sort1()
    {
        sort(copy_planes); 
    }

    void Draw()
    {
       vector<coordinate3f> tj = { baseCenter };
       test.DrawPoint(tj,coordinate3f(0,0,0),50);
       
       cout << watch->rotx << "    " << watch->roty << "    " << watch->rotz << "    "<<endl;
       
       
       copy_planes.clear();
       copy_planes.insert(copy_planes.end(), planes.begin(), planes.end());
       watch->buildhand();
       for (int i = 0; i < watch->planes2.size(); i++)
       {
           watch->planes2[i] = watch->planes2[i].rotate(watch->rotx, 1, 0, 0, watch->clockCenter);
           watch->planes2[i] = watch->planes2[i].rotate(watch->roty, 0, 1, 0, watch->clockCenter);
           watch->planes2[i] = watch->planes2[i].rotate(watch->rotz, 0, 0, 1, watch->clockCenter);
       }

       copy_planes.insert(copy_planes.end(), watch->planes2.begin(), watch->planes2.end());

       sort(copy_planes);
        
      /*glEnable(GL_DEPTH_TEST);
       glDepthMask(GL_TRUE);*/
       for (auto i : copy_planes)
       {
           i.vertex[0].z=i.vertex[1].z=i.vertex[2].z=0;
           i.drawPlane();
           i.drawMesh();
       }
       vector<coordinate3f> p = { watch->clockCenter };
       watch->clockCenter.print();
       DrawPoint(p,0,10);
    }
};