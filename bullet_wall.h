#ifndef BULLET_WALL_STICK_H
#define BULLET_WALL_STICK_H

#include "bullet.h"
#include "game.h"
#include <vector>

class Bullet_Wall_Data {
public:
    Bullet_Wall_Data();
    Bullet_Wall_Data(bool infinite_bounce);
    Bullet_Wall_Data(double v, double a);
    //Bullet_Wall_Data(Character* player, double v, double a);
    static const short freeze = 0;
    static const short vertical = 1;
    static const short bounce = 2;
    //static const short shootAtPlayer = 3;
    friend class Bullet_Wall;
private:
    short type;
    union {
        struct {
            double v,a;
        } vertical;
        struct {
            bool infinite;
        } bounce;
    } data;
};

class Bullet_Wall : public Bullet
{
public:
    Bullet_Wall(QString img, int radius, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0);
    void move();
    Bullet_Wall& addWallData();
    Bullet_Wall& addWallData(bool infinite_bounce);
    Bullet_Wall& addWallData(double v, double a);
    //Bullet_Wall& addWallData(Character* player, double v, double a);
private:
    bool already_enter;
    std::vector<Bullet_Wall_Data> bullet_wall_data_list;
};

#endif // BULLET_WALL_STICK_H
