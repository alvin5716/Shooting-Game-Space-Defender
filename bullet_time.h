#ifndef BULLET_TIME_H
#define BULLET_TIME_H

#include "bullet.h"
#include <vector>

class Bullet_Time_Data {
public:
    Bullet_Time_Data(int wait_time);
    Bullet_Time_Data(int wait_time, double xv, double yv, double xa, double ya);
    Bullet_Time_Data(int wait_time, Character* player, double v, double a);
    static const short freeze = 0;
    static const short updateVA = 1;
    static const short shootAtPlayer = 2;
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
    } data;
};

class Bullet_Time: public Bullet
{
public:
    Bullet_Time(QString img, int radius, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0);
    void move();
    Bullet_Time& addTimeData(int wait_time);
    Bullet_Time& addTimeData(int wait_time, double xv, double yv, double xa, double ya);
    Bullet_Time& addTimeData(int wait_time, Character* player, double v, double a);
private:
    std::vector<Bullet_Time_Data> bullet_time_data_list;
    int timer;
};

#endif // BULLET_TIME_H
