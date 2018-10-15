#ifndef ENEMY_BLUE_1_H
#define ENEMY_BLUE_1_H

#include "enemy_blue.h"
#include <QString>
#include <vector>

class Enemy_Blue_1 : public Enemy_Blue
{
public:
    Enemy_Blue_1(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    void skill();
    std::vector<Bullet*>* shoot2();
private:
    bool clockwise, bullet_fast;
    double angle;
};

#endif // ENEMY_BLUE_1_H
