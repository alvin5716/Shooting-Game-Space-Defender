#ifndef BULLET_STATIC_H
#define BULLET_STATIC_H

#include "bullet.h"
#include <vector>

class Bullet_Nether : public Bullet
{
public:
    Bullet_Nether(QString img, int radius, Character* nether_center, int lifetime=1000, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0);
    QPropertyAnimation *fadeoutAni;
    void setFadeoutTime(int time);
    bool outOfFrame() override;
public slots:
    void move();
private:
    int lifetimer, fadeout_time;
    Character* nether_center;
    QGraphicsOpacityEffect *eff;
};

#endif // BULLET_STATIC_H
