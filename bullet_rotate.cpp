#include "bullet_rotate.h"
#include <QString>
#include <QDebug>

Bullet_Rotate::Bullet_Rotate(QString img, double xc, double yc, double ac, bool clockwise, int radius, double x, double y, double xv, double yv, double xa, double ya)
    :Bullet(img,radius,x,y,xv,yv,xa,ya),xc(xc),yc(yc),ac(ac),clockwise(clockwise)
{
}

void Bullet_Rotate::move() {
    //move object
    double sin, cos, angle;
    angle = angleofvector(xc-x,yc-y);
    cos = std::cos(angle+((clockwise)?(-M_PI/2):(M_PI/2)));
    sin = std::sin(angle+((clockwise)?(-M_PI/2):(M_PI/2)));
    setSpeed(xv+xa,yv+ya);
    setPosition(x+xv,y+yv);
    setAcceleration(ac*cos,ac*sin);
}
