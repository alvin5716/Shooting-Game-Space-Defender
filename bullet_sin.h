#ifndef BULLET_SIN_H
#define BULLET_SIN_H

#include "bullet.h"

class Bullet_Sin :public Bullet
{
public:
    Bullet_Sin(const QString &img, int T, int r, int radius, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool sin_or_cos=true);
public slots:
    void move();
private:
    double xc, yc, r, T, t;
    bool sin_or_cos;
};

#endif // BULLET_SIN_H
