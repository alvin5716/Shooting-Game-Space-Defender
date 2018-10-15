#ifndef ENEMY_GREEN_H
#define ENEMY_GREEN_H

#include "enemy.h"

class Enemy_Green: public Enemy
{
public:
    Enemy_Green(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
};

#endif // ENEMY_GREEN_H
