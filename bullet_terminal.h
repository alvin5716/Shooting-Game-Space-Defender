#ifndef BULLET_TERMINAL_H
#define BULLET_TERMINAL_H

#include "bullet.h"

class Bullet_Terminal : public Bullet
{
public:
    Bullet_Terminal(QString img, int radius, double terminal_v,double x=0,double y=0,double xv=0,double yv=0,double xa=0,double ya=0);
public slots:
    void move();
private:
    double terminal_v;
};

#endif // BULLET_TERMINAL_H
