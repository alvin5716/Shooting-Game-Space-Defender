#ifndef ENEMY_4_H
#define ENEMY_4_H

#include "enemy.h"

class Enemy_4: public Enemy
{
public:
    Enemy_4(QString img, int img_w, int img_h, int show_w, int show_h, Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    void setInvulnerable() override;
protected:
    double move_speed;
    int prep_timer;
    bool invulnerable_after_init;
};

#endif // ENEMY_4_H
