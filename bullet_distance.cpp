#include "bullet_distance.h"
#include <cmath>

Bullet_Distance::Bullet_Distance(QString img, int radius, Character* center, int distance, double x,double y,double xv,double yv,double xa,double ya)
    :Bullet(img,radius,x,y,xv,yv,xa,ya),center(center),distance(distance)
{
    isInArea=false;
}
void Bullet_Distance::move() {
    Bullet::move();
    if(!isInArea && sqrt(std::pow(x-center->getX(),2)+std::pow(y-center->getY(),2)) <= distance) {
        emit enterArea();
        isInArea=true;
    } else if(isInArea && sqrt(std::pow(x-center->getX(),2)+std::pow(y-center->getY(),2)) > distance) {
        emit leaveArea();
        isInArea=false;
    }
}
