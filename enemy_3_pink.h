#ifndef ENEMY_3_PINK_H
#define ENEMY_3_PINK_H

#include "enemy_3.h"

class Enemy_3_Pink: public Enemy_3
{
public:
    Enemy_3_Pink(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    std::vector<Bullet*>* shoot();
private:
    double angle;
    bool mode;
};

#endif // ENEMY_3_PINK_H
