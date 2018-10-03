#ifndef BULLET_H
#define BULLET_H
#include "character.h"
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class BulletData;
class BulletDataTime;
class BulletDataWall;
class BulletDataContinuous;

class Bullet : public Character
{
    Q_OBJECT
public:
    Bullet(QString img, int radius, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0);
    void setVTerminal(double terminal_v);
    // time data
    Bullet* addTimeData(int wait_time);
    Bullet* addTimeData(int wait_time, int aim_radius);
    Bullet* addTimeData(int wait_time, double xv, double yv, double xa, double ya);
    Bullet* addTimeData(int wait_time, Character* player, double v, double a);
    Bullet* addTimeData(int wait_time, double x, double y, int time);
    // wall data
    Bullet* addWallData();
    Bullet* addWallData(bool infinite_bounce, bool bounce_when_hit_downside=false);
    Bullet* addWallData(double vertical_v, double vertical_a);
    Bullet* addWallData(Character* player, double v);
    Bullet* addWallData(Character* player, double v, double a);
    // continuous data
    Bullet* rotateAround(Character* rotate_center, double rotate_ac, bool clockwise);
    Bullet* rotateAround(int rotate_xc, int rotate_yc, double rotate_ac, bool clockwise);
    /*
    Bullet* gravityFrom(Character* rotate_center, double gravity_ac);
    Bullet* gravityFrom(int gravity_xc, int gravity_yc, double gravity_ac);
    */
    Bullet* moveAsTrigFunction(int T, int r, bool sin_or_cos);
    Bullet* moveAsPeriodicFunction(int T, int r, double (*periodic_func)(double));
    ~Bullet();
    friend class BulletDataTime;
    friend class BulletDataWall;
    friend class BulletDataContinuous;
public slots:
    virtual void move();
signals:
    void triggered();
protected:
    void addData(BulletData* bullet_data);
    double terminal_v;
    bool setPositionByData;
    BulletData* data_head;
};

#endif // BULLET_H
