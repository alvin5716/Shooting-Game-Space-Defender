#ifndef BULLET_WALL_STICK_H
#define BULLET_WALL_STICK_H

#include "bullet.h"
#include "game.h"

class Bullet_Wall_Stick : public Bullet
{
public:
    Bullet_Wall_Stick(QString img, int radius, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0);
    void move();
private:
    bool already_enter;
};


#endif // BULLET_WALL_STICK_H
