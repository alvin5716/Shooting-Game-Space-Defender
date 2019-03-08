#ifndef ENEMY_4_BLUE_6_H
#define ENEMY_4_BLUE_6_H

#include "enemy_4_blue.h"
#include "bullet_distance.h"

class Enemy_4_Blue_6 :public Enemy_4_Blue
{
    Q_OBJECT
public:
    Enemy_4_Blue_6(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    void skill() override;
    std::vector<Bullet*>* shoot2() override;
};

#endif // ENEMY_4_BLUE_6_H
