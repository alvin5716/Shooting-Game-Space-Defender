#include "bullet_distance.h"
#include <cmath>
#include <QDebug>

Bullet_Distance::Bullet_Distance(QString img, int radius, Character* center, int distance, double x,double y,double xv,double yv,double xa,double ya)
    :Bullet(img,radius,x,y,xv,yv,xa,ya),distance(distance)
{
    if(center!=nullptr) centers.push_back(center);
    isInArea=false;
    disabled_timer=0;
}

void Bullet_Distance::addNewCenter(Character *center) {
    this->centers.push_back(center);
}

void Bullet_Distance::move() {
    Bullet::move();
    if(disabled_timer<=0) {
        if(!isInArea) {
            for(Character* center: centers) {
                if(sqrt(std::pow(x-center->getX(),2)+std::pow(y-center->getY(),2)) <= distance) {
                    emit enterArea();
                    isInArea=true;
                    break;
                }
            }
        } else {
            bool inTest = false;
            for(Character* center: centers) {
                if(sqrt(std::pow(x-center->getX(),2)+std::pow(y-center->getY(),2)) <= distance) {
                    inTest=true;
                    break;
                }
            }
            if(!inTest) {
                emit leaveArea();
                isInArea=false;
            }
        }
    } else {
        if(isInArea) {
            emit leaveArea();
            isInArea=false;
        }
        --disabled_timer;
    }
}
void Bullet_Distance::disable(int time) {
    disabled_timer=time;
}
