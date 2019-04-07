#ifndef BULLETDATATIME_H
#define BULLETDATATIME_H

#include "bulletdata.h"

enum class BulletDataTimeType{freeze, zoom, updateVA, shootAtPlayer, moveTo};

class BulletDataTime : public BulletData
{
public:
    BulletDataTime(Bullet* bullet, int wait_time);
    BulletDataTime(Bullet* bullet, int wait_time, int aim_radius);
    BulletDataTime(Bullet* bullet, int wait_time, double xv, double yv, double xa, double ya);
    BulletDataTime(Bullet* bullet, int wait_time, Player* player, double v, double a);
    BulletDataTime(Bullet* bullet, int wait_time, double x, double y, int time);
    bool skill() override;
    ~BulletDataTime() override;
private:
    int wait_time;
    BulletDataTimeType type;
    union {
        struct {
            int aim_radius;
            double now_radius, increment;
        } zoom;
        struct {
            double xv, yv, xa, ya;
        } updateVA;
        struct {
            double v, a;
            Player* player;
        } shootAtPlayer;
        struct {
            double x, y;
            int time;
        } moveTo;
    } data;
};

#endif // BULLETDATATIME_H
