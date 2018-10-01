#ifndef BULLET_H
#define BULLET_H
#include "character.h"
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include "bulletdatatime.h"

class Bullet : public Character
{
    Q_OBJECT
public:
    Bullet(QString img, int radius, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0);
    void setVTerminal(double terminal_v);
    Bullet* addTimeData(int wait_time);
    Bullet* addTimeData(int wait_time, double xv, double yv, double xa, double ya);
    Bullet* addTimeData(int wait_time, Character* player, double v, double a);
    Bullet* addTimeData(int wait_time, double x, double y, int time);
public slots:
    virtual void move();
signals:
    void triggered();
protected:
    void addData(BulletData* bullet_data);
    double terminal_v;
    BulletData* data_head;
};

#endif // BULLET_H
