#ifndef BULLET_BOUNCE_H
#define BULLET_BOUNCE_H

#include "bullet.h"

class Bullet_Bounce : public Bullet
{
public:
    Bullet_Bounce(QString img, int bounce_time, int radius, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0);
    void move();
private:
    int bounce_time;
    bool already_enter;
};

#endif // BULLET_BOUNCE_H
