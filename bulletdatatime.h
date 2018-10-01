#ifndef BULLETDATATIME_H
#define BULLETDATATIME_H

#include "bulletdata.h"

enum class BulletDataTimeType{freeze, updateVA, shootAtPlayer, moveTo};

class BulletDataTime : public BulletData
{
public:
    BulletDataTime(Character* bullet, int wait_time);
    BulletDataTime(Character* bullet, int wait_time, double xv, double yv, double xa, double ya);
    BulletDataTime(Character* bullet, int wait_time, Character* player, double v, double a);
    BulletDataTime(Character* bullet, int wait_time, double x, double y, int time);
    bool skill() override;
    ~BulletDataTime() override;
private:
    int wait_time;
    BulletDataTimeType type;
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

#endif // BULLETDATATIME_H
