#ifndef ENEMY_4_PINK_H
#define ENEMY_4_PINK_H

#include "enemy_4.h"

class Enemy_4_Pink: public Enemy_4
{
public:
    Enemy_4_Pink(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool stopable=false);
    void attacked() override;
    void skill() override;
private:
    int shoot_cd_init;
};

#endif // ENEMY_4_PINK_H
