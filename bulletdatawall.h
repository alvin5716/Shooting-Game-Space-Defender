#ifndef BULLETDATAWALL_H
#define BULLETDATAWALL_H

#include "bulletdata.h"

enum class BulletDataWallType{freeze, vertical, bounce, magicStone, shootAtPlayer};

class BulletDataWall : public BulletData
{
public:
    BulletDataWall(Bullet* bullet);
    BulletDataWall(Bullet* bullet, bool infinite_bounce, bool bounce_when_hit_downside);
    BulletDataWall(Bullet* bullet, double vertical_v, double vertical_a);
    BulletDataWall(Bullet* bullet, Player* player, double v);
    BulletDataWall(Bullet* bullet, Player* player, double v, double a);
    bool skill() override;
    ~BulletDataWall() override;
private:
    bool already_enter;
    BulletDataWallType type;
    union {
        struct {
            double v,a;
        } vertical;
        struct {
            bool infinite, bounce_when_hit_downside;
        } bounce;
        struct {
            Player* player;
            double v;
        } magicStone;
        struct {
            Player* player;
            double v;
            double a;
        } shootAtPlayer;
    } data;
};

#endif // BULLETDATAWALL_H
