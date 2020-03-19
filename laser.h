#ifndef LASER_H
#define LASER_H

#include "bullet.h"

class Enemy;

class Laser : public Bullet
{
public:
    Laser(int pixmap, Enemy* shooter, int radius, double angle, double omega=0, int lifetime=63, double x=0, double y=0, int prepare_time=75);
    Laser(const QString &img, Enemy* shooter, int radius, double angle, double omega=0, int lifetime=63, double x=0, double y=0, int prepare_time=75);
    Character* testAttackedBy(Character* attacker) override;
    double getAngle() const;
    void setSoundEnabled(bool sound_enabled=true);
public slots:
    void move() override;
    void img_move() override;
private:
    double angle, omega;
    int prepare_timer, dead_timer;
    bool preparing, dying, sound_enabled;
};

#endif // LASER_H
