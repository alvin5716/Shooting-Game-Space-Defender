#ifndef BULLET_DISTANCE_H
#define BULLET_DISTANCE_H

#include "bullet.h"
#include <vector>

class Bullet_Distance : public Bullet
{
    Q_OBJECT
public:
    Bullet_Distance(QString img, int radius, Character* center, int distance, double x=0,double y=0,double xv=0,double yv=0,double xa=0,double ya=0);
public slots:
    void move();
    void disable(int time=250);
    void addNewCenter(Character* center);
signals:
    void enterArea();
    void leaveArea();
private:
    std::vector<Character*> centers;
    int distance, disabled_timer;
    bool isInArea;
};

#endif // BULLET_DISTANCE_H
