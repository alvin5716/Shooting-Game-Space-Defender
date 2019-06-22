#ifndef ENEMY_4_BLUE_2_H
#define ENEMY_4_BLUE_2_H

#include "spreadrng.h"
#include "enemy_4_blue.h"

class Enemy_4_Blue_2 :public Enemy_4_Blue
{
public:
    Enemy_4_Blue_2(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    void skill() override;
    std::vector<Bullet*>* shoot2() override;
    ~Enemy_4_Blue_2();
private:
    short shoot_count;
    static constexpr int shoot_times=5;
    double angle[shoot_times], xp, yp;
    int xd[shoot_times];
    SpreadRNG* rng;
};

#endif // ENEMY_4_BLUE_2_H
