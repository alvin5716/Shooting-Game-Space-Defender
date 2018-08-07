#ifndef BULLET_TIME_H
#define BULLET_TIME_H

#include "bullet.h"
#include <vector>

class Bullet_Time_Data {
public:
    Bullet_Time_Data(int wait_time);
    Bullet_Time_Data(int wait_time, double xv, double yv, double xa, double ya);
    Bullet_Time_Data(int wait_time, Character* player, double v, double a);
    Bullet_Time_Data(int wait_time, double x, double y, int time);
    static const short freeze = 0;
    static const short updateVA = 1;
    static const short shootAtPlayer = 2;
    static const short moveTo = 3;
    friend class Bullet_Time;
private:
    int wait_time;
    short type;
    union {
        struct {
            double xv, yv, xa, ya;
        } updateVA;
        struct {
            double v, a;
            Character* player;
        } shootAtPlayer;
        struct {
            double x, y;
            int time;
        } moveTo;
    } data;
};

class Bullet_Time: public Bullet
{
    Q_OBJECT
public:
    Bullet_Time(QString img, int radius, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0);
    void move();
    Bullet_Time& addTimeData(int wait_time);
    Bullet_Time& addTimeData(int wait_time, double xv, double yv, double xa, double ya);
    Bullet_Time& addTimeData(int wait_time, Character* player, double v, double a);
    Bullet_Time& addTimeData(int wait_time, double x, double y, int time);
signals:
    void triggered();
private:
    std::vector<Bullet_Time_Data> bullet_time_data_list;
    int timer;
};

#endif // BULLET_TIME_H
