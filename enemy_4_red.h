#ifndef ENEMY_4_RED_H
#define ENEMY_4_RED_H

#include "enemy_4.h"

class Enemy_4_Red: public Enemy_4
{
public:
    Enemy_4_Red(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    void skill() override;
    std::vector<Bullet*>* shoot() override;
};

#endif // ENEMY_4_RED_H
