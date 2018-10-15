#ifndef ENEMY_3_BLUE_4_H
#define ENEMY_3_BLUE_4_H

#include "enemy_3_blue.h"
#include <QString>
#include <vector>

class Enemy_3_Blue_4 : public Enemy_3_Blue
{
public:
    Enemy_3_Blue_4(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    void skill();
    std::vector<Bullet*>* shoot2();
private:
    double angle, angle2;
    int shoot_timer_2;
};

#endif // ENEMY_3_BLUE_4_H
