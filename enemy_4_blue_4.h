#ifndef ENEMY_4_BLUE_4_H
#define ENEMY_4_BLUE_4_H

#include "enemy_4_blue.h"

class Enemy_4_Blue_4 :public Enemy_4_Blue
{
public:
    Enemy_4_Blue_4(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    void skill();
    std::vector<Bullet*>* shoot2();
};

#endif // ENEMY_4_BLUE_4_H
