#include "bullet.h"
#include <cmath>
#include <QDebug>

Bullet::Bullet(QString img, int radius, double x, double y, double xv, double yv, double xa, double ya)
    :Character(img,50,50,(int)round(radius*2.2),(int)round(radius*2.2),1,radius,x,y,xv,yv,xa,ya)
{
    terminal_v = 0;
}

void Bullet::setVTerminal(double terminal_v) {
    this->terminal_v = terminal_v;
}

void Bullet::move() {
    if(terminal_v==0) {
        Character::move();
    } else {
        //move object
        setPosition(x+xv,y+yv);
        if((sqrt(pow(xv,2)+pow(yv,2))<=terminal_v && sqrt(pow(xv+xa,2)+pow(yv+ya,2))<=terminal_v) || (sqrt(pow(xv,2)+pow(yv,2))>=terminal_v && sqrt(pow(xv+xa,2)+pow(yv+ya,2))>=terminal_v)) setSpeed(xv+xa,yv+ya);
    }
}
