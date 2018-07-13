#ifndef BULLET_WALL_H
#define BULLET_WALL_H

#include "bullet.h"

class Bullet_Wall : public Bullet
{
public:
    Bullet_Wall(QString img, int radius, int bounce_time, double vt, double at, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0);
    void move();
private:
    bool already_enter;
    int bounce_time;
    double vt, at;
};

#endif // BULLET_WALL_H
