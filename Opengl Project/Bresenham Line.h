#include"putpixel.h"

void Bresenham_Line(coordinate2f a, coordinate2f b, coordinate3f color)
{
    vector<coordinate2f> meshpoint = {};
    coordinate2f temp;
    int x1 = a.x, x2 = b.x;
    int y1 = a.y, y2 = b.y;
    //calculate slope and exchange x and y if the m>1
    bool SWAP = 0;
    if (abs(y2 - y1) > abs(x2 - x1))
    {
        swap(x1, y1);
        swap(x2, y2);
        SWAP = 1;
    }

    //Start from the small x
    if (x1 > x2)
    {
        swap(x1, x2);
        swap(y1, y2);
    }

    //Calculate dy dy and initial decision parameter p
    int dy = abs(y2 - y1);
    int dx = abs(x2 - x1);
    int yinc = (y2 > y1) ? 1 : -1;
    int p = 2 * dy - dx;

    while ((x2 - x1) > 0)
    {
        if (!SWAP)
            temp = coordinate2f(x1, y1);
        else
            temp = coordinate2f(y1, x1);

        if (p < 0)
            p += 2 * dy;
        else
        {
            p += 2 * (dy - dx);
            y1 += yinc;
        }
        x1 += 1;
        putpixel(temp, color);
    }
}