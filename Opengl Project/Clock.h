#pragma once
#include"plane.h"
class Clock
{
    GLfloat edge;
    time_t now;
    tm* Time;
    vector<coordinate3f> dot;
    vector<coordinate3f> hand_coordinates;
    int radius;
    
    void build()
    {
        planes.clear();
        vector<coordinate3f> circle[2];
        coordinate3f temp1,temp2;
        vector<plane> temp3;
        GLfloat sqRadius = radius*0.05;
        
        for (GLfloat i = 0; i < 2 * pi; i += 2 * pi / 12)
        {
            temp1 = coordinate3f(radius * 0.9 * cos(i), radius * 0.85 * sin(i));
            temp2=clockCenter + temp1 + coordinate3f(0, 0, edge*1.2);
            vector<coordinate3f> p = {temp2 + coordinate3f(-sqRadius,+sqRadius,0), temp2 + coordinate3f(+sqRadius, +sqRadius, 0) };
            vector<coordinate3f> q = {temp2 + coordinate3f(-sqRadius,-sqRadius,0), temp2 + coordinate3f(+sqRadius, -sqRadius, 0) };
            temp3 = genPlane(p, q,clockCenter,coordinate3f());
            planes.insert(planes.end(), temp3.begin(), temp3.end());
        }
                
        GLfloat factor = 1.1;
        coordinate3f temp;
        
        for (GLfloat i = 0; i < pi; i += pi / 20)
        {
            temp = coordinate3f(radius * cos(i), radius * sin(i), 0);
            circle[0].push_back(clockCenter + temp + coordinate3f(0, 0, edge * factor));
            circle[1].push_back(clockCenter + coordinate3f(temp.x, -temp.y, temp.z) + coordinate3f(0, 0, edge * factor));
        }

        vector<plane> p= genPlane(circle[0], circle[1],clockCenter);
        planes.insert(planes.end(), p.begin(), p.end());
        int size = planes.size();

        for (int i=0;i<size;i++)
        {
            planes.push_back(planes[i].rotate(-90, 0, 1, 0, clockCenter));
            planes.push_back(planes[i].rotate(+90, 0, 1, 0, clockCenter));
            planes.push_back(planes[i].rotate(180, 0, 1, 0, clockCenter));
        }
        buildhand();
    }

public:
    coordinate3f clockCenter, *pivot,handCenter;
    GLint rotx, roty, rotz;
    void buildhand()
    {
        coordinate3f handAngle, h[2], m[2], s[2];
        planes2.clear();

        now = time(0);
        Time = localtime(&now);
        handAngle = coordinate3f(Time->tm_hour * 360 / 12,
            Time->tm_min * 360 / 60,
            Time->tm_sec * 360 / 60);

        
        h[0] = coordinate3f(0.5 * radius * sin(handAngle.x * pi / 180), 0.5 * radius * cos(handAngle.x * pi / 180), 0);
        m[0] = coordinate3f(0.7 * radius * sin(handAngle.y * pi / 180), 0.7 * radius * cos(handAngle.y * pi / 180), 0);
        s[0] = coordinate3f(0.8 * radius * sin(handAngle.z * pi / 180), 0.8 * radius * cos(handAngle.z * pi / 180), 0);


        h[1] = h[0].rotation(-5, 0, 0, 1);
        m[1] = m[0].rotation(-5, 0, 0, 1);
        s[1] = s[0].rotation(-5, 0, 0, 1);

        planes2.push_back(plane(handCenter+ h[0], handCenter + h[1], handCenter, clockCenter, coordinate3f()));
        planes2.push_back(plane(handCenter, handCenter + h[1] - h[0], handCenter + h[1],clockCenter, coordinate3f()));

        planes2.push_back(plane(handCenter + m[0], handCenter + m[1], handCenter, clockCenter, coordinate3f()));
        planes2.push_back(plane(handCenter, handCenter + m[1] - m[0], handCenter + m[1], clockCenter, coordinate3f()));

        planes2.push_back(plane(handCenter + s[0], handCenter + s[1], handCenter, clockCenter, coordinate3f()));
        planes2.push_back(plane(handCenter, handCenter + s[1] - s[0], handCenter + s[1], clockCenter, coordinate3f()));
     
        int size = planes2.size();
        for (int i = 0; i < size; i++)
        {
            planes2[i] = planes2[i].translate(clockCenter);
            planes2.push_back(planes2[i].rotate(-90, 0, 1, 0,clockCenter));
            planes2.push_back(planes2[i].rotate(+90, 0, 1, 0,clockCenter));
            planes2.push_back(planes2[i].rotate(+180, 0, 1, 0,clockCenter));
        }


        vector<coordinate3f> transform = {
                                    coordinate3f(1,0,0),
                                    coordinate3f(0,1,0),
                                    coordinate3f(0,0,1)
        };

    }

    vector<plane> planes2;
    vector<plane> planes;
    Clock(int _radius = 0, coordinate3f _clockCenter = coordinate3f(), GLfloat _edge = 0,coordinate3f *_pivot=new coordinate3f())
    {
        rotx = roty = rotz = 0;
        pivot = _pivot;
        radius = 0.9 * _radius;
        edge = _edge;
        clockCenter = _clockCenter;
        handCenter = coordinate3f(0, 0, edge * 1.2);
        build();
    }
};