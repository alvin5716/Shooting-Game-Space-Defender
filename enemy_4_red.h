#ifndef ENEMY_4_RED_H
#define ENEMY_4_RED_H

#include "enemy_4.h"

class Enemy_4_Red: public Enemy_4
{
public:
    Enemy_4_Red(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool single_attack=false);
    void skill() override;
    std::vector<Bullet*>* shoot() override;
private:
    bool single_attack, leaving;
    static const int move_time=130;
};

#endif // ENEMY_4_RED_H
