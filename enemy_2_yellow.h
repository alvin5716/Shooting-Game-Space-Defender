#ifndef ENEMY_2_YELLOW_H
#define ENEMY_2_YELLOW_H

#include "enemy.h"

class Enemy_2_Yellow : public Enemy
{
public:
    Enemy_2_Yellow(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    std::vector<Bullet*>* shoot();
private:
    double cosa, sina;
};

#endif // ENEMY_2_YELLOW_H
