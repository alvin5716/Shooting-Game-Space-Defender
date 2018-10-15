#ifndef ENEMY_4_BLUE_H
#define ENEMY_4_BLUE_H

#include "enemy_4.h"

class Enemy_4_Blue :public Enemy_4
{
    Q_OBJECT
public:
    Enemy_4_Blue(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    std::vector<Bullet*>* shoot() override;
public slots:
    void deadSet();
};

#endif // ENEMY_4_BLUE_H
