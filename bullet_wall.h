#ifndef BULLET_WALL_STICK_H
#define BULLET_WALL_STICK_H

#include "bullet.h"
#include "game.h"
#include <vector>

enum class Bullet_Wall_Data_Type{freeze, vertical, bounce, magicStone, shootAtPlayer};

class Bullet_Wall_Data {
public:
    Bullet_Wall_Data(Bullet_Wall_Data_Type bullet_wall_data_type);
    Bullet_Wall_Data(bool infinite_bounce, bool bounce_when_hit_downside);
    Bullet_Wall_Data(double vertical_v, double vertical_a);
    Bullet_Wall_Data(Character* player, double v, double aim_radius, bool zoom);
    Bullet_Wall_Data(Character* player, double v, double a);
    friend class Bullet_Wall;
private:
    Bullet_Wall_Data_Type type;
    union {
        struct {
            double v,a;
        } vertical;
        struct {
            bool infinite, bounce_when_hit_downside;
        } bounce;
        struct {
            Character* player;
            double v;
            bool zooming;
            int aim_radius;
        } magicStone;
        struct {
            Character* player;
            double v;
            double a;
        } shootAtPlayer;
    } data;
};

class Bullet_Wall : public Bullet
{
    Q_OBJECT
public:
    Bullet_Wall(QString img, int radius, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0);
    void move();
    Bullet_Wall& addWallData(Bullet_Wall_Data_Type bullet_wall_data_type=Bullet_Wall_Data_Type::freeze);
    Bullet_Wall& addWallData(bool infinite_bounce, bool bounce_when_hit_downside=false);
    Bullet_Wall& addWallData(double vertical_v, double vertical_a);
    Bullet_Wall& addWallData(Character* player, double v);
    Bullet_Wall& addWallData(Character* player, double v, double a);
private:
    bool already_enter;
    std::vector<Bullet_Wall_Data> bullet_wall_data_list;
};

#endif // BULLET_WALL_STICK_H
