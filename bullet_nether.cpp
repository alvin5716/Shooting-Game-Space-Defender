#include "bullet_nether.h"
#include "effect.h"

Bullet_Nether::Bullet_Nether(int pixmap, int radius, Character* nether_center, int lifetime, double x, double y, double xv, double yv, double xa, double ya)
    :Bullet(pixmap,radius,x,y,xv,yv,xa,ya)
{
    lifetimer=lifetime;
    this->nether_center=nether_center;
    fadeout_time=500;
}
Bullet_Nether::Bullet_Nether(const QString &img, int radius, Character* nether_center, int lifetime, double x, double y, double xv, double yv, double xa, double ya)
    :Bullet(img,radius,x,y,xv,yv,xa,ya)
{
    lifetimer=lifetime;
    this->nether_center=nether_center;
    fadeout_time=500;
}
void Bullet_Nether::move() {
    //move object
    Bullet::move();
    //attack by nether center
    if(nether_center!=nullptr) {
        if(sqrt(pow(nether_center->getX() - x,2)+pow(nether_center->getY() - y,2)) <= nether_center->getRadius() + radius) {
            if(health>0 && !invulnerable) health-=1;
            if(health<=0 && !invulnerable) {
                fadeout(fadeout_time);
                lifetimer=500/8;
                setInvulnerable();
            }
        }
    }
    //lifetimer
    --lifetimer;
    if(lifetimer<=0) dead=true;
}
bool Bullet_Nether::outOfFrame() {
    return false;
}
void Bullet_Nether::setFadeoutTime(int time) {
    this->fadeout_time=time;
}
