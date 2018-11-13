#ifndef ENEMY_4_BLUE_2_H
#define ENEMY_4_BLUE_2_H

#include "enemy_4_blue.h"

class Enemy_4_Blue_2 :public Enemy_4_Blue
{
public:
    Enemy_4_Blue_2(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    void skill();
    std::vector<Bullet*>* shoot2();
private:
    short shoot_count;
    static constexpr int shoot_times=5;
    double angle[shoot_times], xp, yp;
    int xd[shoot_times];
};

#endif // ENEMY_4_BLUE_2_H
