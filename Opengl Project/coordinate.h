#pragma once
#include<iostream>
#include<math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include<vector>


#define pi 3.14159
//+->addition
// -=>subtraction
//^->dot product
//*->cross product
//~->|vector|
//!->unit vector

class coordinate3f
{
public:
    GLfloat x, y, z;
    coordinate3f(float _x = 0, float _y = 0, float _z = 0){
        x = _x; y = _y; z = _z;
    }
    void print()
    {
        std::cout << x << "    " << y << "   " << z<<std::endl;
    }
    coordinate3f operator + (coordinate3f t) {
        return coordinate3f(x + t.x, y + t.y, z + t.z);
    }
    coordinate3f operator - (coordinate3f t) {
        return coordinate3f(x - t.x, y - t.y, z - t.z);
    }
    GLfloat operator ^ (coordinate3f t) {
        return (x * t.x+y * t.y+z * t.z);
    }
    coordinate3f operator * (coordinate3f t) {
        GLfloat b1 = x, b2 = y, b3 = z;
        GLfloat c1 = t.x,c2 = t.y, c3 = t.z;
        return coordinate3f(b2 * c3 - b3 * c2, b3 * c1 - b1 * c3, b1 * c2 - b2 * c1);
    }

    coordinate3f operator % (float m) {
        return coordinate3f(m * x, m * y, m * z);
    }

    coordinate3f operator / (GLfloat t) {
        return coordinate3f(x / t, y / t, z / t);
    }

    coordinate3f operator ! (){
        GLfloat len = sqrt(x * x + y * y + z * z);
        if (len == 0)
            return coordinate3f(0,0,0);

        return *this / len;
    }

    bool operator == (coordinate3f t){
        return (x == t.x) && (y == t.y) && (z == t.z);
    }

    coordinate3f integer()
    {
        return coordinate3f(int(x+0.5), int(y+0.5), int(z+0.5));
    }

    coordinate3f rotation(GLfloat alpha, bool _x ,bool _y,bool _z,coordinate3f pivot = coordinate3f())
    {
        alpha*=pi / 180;
        coordinate3f temp = *this-pivot;
        GLfloat cosine = cos(alpha);
        GLfloat sine = sin(alpha);
        if (_x)
        {
            coordinate3f R[3] = {
                            coordinate3f(1,              0,              0 ),
                            coordinate3f(0,              cosine,     -sine ),
                            coordinate3f(0,              sine,       cosine),
            };
            temp = coordinate3f(R[0] ^ temp, R[1] ^ temp, R[2] ^ temp);
        }

        else if (_y)
        {
            coordinate3f R[3] = {
                                       coordinate3f(  cosine,        0,       sine      ),
                                       coordinate3f(  0,             1,       0         ),
                                       coordinate3f( -sine,          0,       cosine    ),
            };
            temp = coordinate3f(R[0] ^ temp, R[1] ^ temp, R[2] ^ temp);
        }
        else if (_z)
        {
            coordinate3f R[3] = {
                                        coordinate3f(cosine,         -sine,      0),
                                        coordinate3f(sine,           cosine,     0),
                                        coordinate3f(0,              0,          1)
            };
            temp = coordinate3f(R[0] ^ temp, R[1] ^ temp, R[2] ^ temp);
        }

        temp = temp+pivot;
        return temp;
    }

    coordinate3f scaling(GLfloat sx=1, GLfloat sy=1, GLfloat sz=1, coordinate3f pivot = coordinate3f())
    {
        coordinate3f temp = *this - pivot;
        temp = coordinate3f(temp.x * sx, temp.y * sy, temp.z * sz);
        temp = temp + pivot;
        return temp;
    }
 
    coordinate3f orthographic(bool _x, bool _y, bool _z)
    {
            return (_x * x, _y * y, _z * z);
    }

    GLfloat distance(coordinate3f t)
    {
        t = *this - t;
        return sqrt(t.x*t.x + t.y*t.y + t.z * t.z);
    }
};

class coordinate2f
{
public:
    GLfloat x, y;
    coordinate2f(float _x = 0, float _y = 0)
    {
        x = _x; y = _y;
    }
    coordinate2f(coordinate3f _t)
    {
        x = _t.x;
        y = _t.y;
    }

    
    void print()
    {
        std::cout << x << "    " << y<<std::endl;
    }
    bool operator == (coordinate2f t) {
        return (x == t.x) && (y == t.y);
    }
};

class coordinate2i
{
public:
    GLint x, y;
    coordinate2i(int _x = 0, int _y = 0)
    {
        x = _x; y = _y;
    }
    coordinate2i(coordinate2f _t)
    {
        x = _t.x+0.5;
        y = _t.y+0.5;
    }

    void print()
    {
        std::cout << x << "    " << y<<std::endl;
    }

    bool operator == (coordinate2i t) {
        return (x == t.x) && (y == t.y);
    }
};