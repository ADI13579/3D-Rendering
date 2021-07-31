#pragma once
#include<string>
#include"coordinate.h"
class material
{
public:
    //to be read from the mtl file on the basis of abive material string
    //===============================================
    std::string id;
    float Ns;
    coordinate3f ka;
    coordinate3f kd;
    coordinate3f ks;
    coordinate3f ke;
    coordinate3f Ni;
    float d;
    //===============================================
    void print()
    {
        std::cout << "id: " << id << std::endl;
        std::cout << "Ns :" << Ns << std::endl;
        std::cout << "ka :"; ka.print();
        std::cout << "ks :"; kd.print();
        std::cout << "ke :"; ke.print();
        std::cout << "Ni :"; Ni.print();
        std::cout << "d :" << d << std::endl;
    }
};