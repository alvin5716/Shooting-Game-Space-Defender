#ifndef LASER_H
#define LASER_H

#include "bullet.h"

class Enemy;

class Laser : public Bullet
{
public:
    Laser(int pixmap, Enemy* shooter, int radius, double angle, double omega=0, int lifetime=63, double x=0, double y=0, int prepare_time=75);
    Laser(const QString &img, Enemy* shooter, int radius, double angle, double omega=0, int lifetime=63, double x=0, double y=0, int prepare_time=75);
    Character* testAttackedBy(Character* attacker);
    double getAngle() const;
public slots:
    void move();
    void img_move();
private:
    double angle, omega;
    int lifetimer, prepare_timer, dead_timer;
    bool preparing, dying;
};

#endif // LASER_H
