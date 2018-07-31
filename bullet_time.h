#ifndef BULLET_TIME_H
#define BULLET_TIME_H

#include "bullet.h"
#include "player.h"
#include <vector>

class Bullet_Time_Data {
public:
    Bullet_Time_Data(int duration, double xv, double yv, double xa, double ya);
    Bullet_Time_Data(int duration, Character* player, double v, double a);
    static const short updateVA = 0;
    static const short shootAtPlayer = 1;
    friend class Bullet_Time;
private:
    int duration;
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
    Bullet_Time& addTimeData(int duration, double xv, double yv, double xa, double ya);
    Bullet_Time& addTimeData(int duration, Character* player, double v, double a);
private:
    std::vector<Bullet_Time_Data> bullet_time_data_list;
    int timer;
};

#endif // BULLET_TIME_H
