#ifndef BULLET_ROTATE_H
#define BULLET_ROTATE_H

#include "bullet.h"

class Bullet_Rotate : public Bullet
{
public:
    Bullet_Rotate(QString img, double xc, double yc, double ac, bool clockwise, int radius, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0);
    void move();
private:
    const double xc, yc, ac;
    const bool clockwise;
};

#endif // BULLET_ROTATE_H
