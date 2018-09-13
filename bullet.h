#ifndef BULLET_H
#define BULLET_H
#include "character.h"
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class Bullet : public Character
{
public:
    Bullet(QString img, int radius, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0);
    void setVTerminal(double terminal_v);
public slots:
    virtual void move();
protected:
    double terminal_v;
};

#endif // BULLET_H
