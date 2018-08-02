#ifndef BULLET_WALL_STICK_H
#define BULLET_WALL_STICK_H

#include "bullet.h"
#include "game.h"
#include <vector>

class Bullet_Wall_Data {
public:
    Bullet_Wall_Data(int bullet_wall_data_type);
    Bullet_Wall_Data(bool infinite_bounce, bool bounce_when_hit_downside);
    Bullet_Wall_Data(double vertical_v, double vertical_a);
    Bullet_Wall_Data(Character* player, double v);
    static const short freeze = 0;
    static const short vertical = 1;
    static const short bounce = 2;
    static const short magicStone = 3;
    friend class Bullet_Wall;
private:
    short type;
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
        } magicStone;
    } data;
};

class Bullet_Wall : public Bullet
{
    Q_OBJECT
public:
    Bullet_Wall(QString img, int radius, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0);
    void move();
    Bullet_Wall& addWallData(int bullet_wall_data_type=Bullet_Wall_Data::freeze);
    Bullet_Wall& addWallData(bool infinite_bounce, bool bounce_when_hit_downside=false);
    Bullet_Wall& addWallData(double vertical_v, double vertical_a);
    Bullet_Wall& addWallData(Character* player, double v);
signals:
    void triggered();
private:
    bool already_enter;
    std::vector<Bullet_Wall_Data> bullet_wall_data_list;
};

#endif // BULLET_WALL_STICK_H
