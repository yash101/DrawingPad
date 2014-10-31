#ifndef PIXEL_HXX
#define PIXEL_HXX
#include <string>
typedef unsigned int usint;
class Pixel
{
private:
    usint R;
    usint G;
    usint B;
public:
    void setRed(usint red)
    {
        R = red;
    }

    void setGreen(usint green)
    {
        G = green;
    }

    void setBlue(usint blue)
    {
        B = blue;
    }

    usint getRed()
    {
        return R;
    }

    usint getGreen()
    {
        return G;
    }

    usint getBlue()
    {
        return B;
    }

    Pixel(usint red, usint green, usint blue)
    {
        R = red;
        G = green;
        B = blue;
    }
};

class Point
{
private:
    usint x;
    usint y;
public:
    void setX(usint x_axis)
    {
        x = x_axis;
    }

    void setY(usint y_axis)
    {
        y = y_axis;
    }

    usint getX()
    {
        return x;
    }

    usint getY()
    {
        return y;
    }

    Point(usint x_axis, usint y_axis)
    {
        x = x_axis;
        y = y_axis;
    }
};

class PointPix
{
private:
    std::string color_code;
    usint x;
    usint y;
public:
    void setColor(std::string color)
    {
        color_code = color;
    }

    std::string getColor()
    {
        return color_code;
    }

    void setX(usint x_axis)
    {
        x = x_axis;
    }

    void setY(usint y_axis)
    {
        y = y_axis;
    }

    usint getX()
    {
        return x;
    }

    usint gety()
    {
        return y;
    }

    PointPix(usint x_axis, usint y_axis, std::string color)
    {
        x = x_axis;
        y = y_axis;
        color_code = color;
    }
};

#endif // PIXEL_HXX
