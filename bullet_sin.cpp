#include "bullet_sin.h"
#include <QDebug>

Bullet_Sin::Bullet_Sin(const QString &img, int T, int r, int radius, double x, double y, double xv, double yv, double xa, double ya, bool sin_or_cos)
    :Bullet(img,radius,x,y,xv,yv,xa,ya),r(r),T(T),sin_or_cos(sin_or_cos)
{
    t=0;
    xc=x;
    yc=y;
}
void Bullet_Sin::move() {
    xc+=xv;
    yc+=yv;
    double angle = M_PI/2+angleofvector(xv,yv);
    if(sin_or_cos)
        setPosition(xc+r*std::sin(2*M_PI*t/T)*std::cos(angle),yc+r*std::sin(2*M_PI*t/T)*std::sin(angle));
    else
        setPosition(xc-r*std::sin(2*M_PI*t/T)*std::cos(angle),yc-r*std::sin(2*M_PI*t/T)*std::sin(angle));
    if(++t>T) t=0;
    setSpeed(xv+xa,yv+ya);
}
