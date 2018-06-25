#ifndef LASER_H
#define LASER_H

#include "bullet.h"

class Laser : public Bullet
{
public:
    Laser(QString img, int radius, double angle, double omega=0, int lifetime=125, double x=0, double y=0);
    Character* testAttackedBy(Character* attacker);
public slots:
    void move();
    void img_move();
private:
    double angle, omega;
    int lifetimer, prepare_timer;
    bool preparing;
};

#endif // LASER_H
