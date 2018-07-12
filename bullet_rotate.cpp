#include "bullet_rotate.h"
#include <QString>
#include <QDebug>

Bullet_Rotate::Bullet_Rotate(QString img, double xc, double yc, double ac, bool clockwise, int radius, double x, double y, double xv, double yv, double xa, double ya)
    :Bullet(img,radius,x,y,xv,yv,xa,ya),xc(xc),yc(yc),ac(ac),clockwise(clockwise)
{
}

void Bullet_Rotate::move() {
    //move object
    double sin, cos, phi;
    sincostoxy(sin,cos,xc,yc);
    phi = (clockwise)?(-M_PI/2):(M_PI/2);
    setSpeed(xv+xa,yv+ya);
    setPosition(x+xv,y+yv);
    setAcceleration(ac*(cos*std::cos(phi)-sin*std::sin(phi)),ac*(sin*std::cos(phi)+cos*std::sin(phi)));
}
