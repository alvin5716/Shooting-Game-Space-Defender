#ifndef ENEMY_BLUE_5_H
#define ENEMY_BLUE_5_H


#include "enemy_blue.h"
#include "bullet_nether.h"
#include <QString>
#include <vector>

class Enemy_Blue_5 : public Enemy_Blue
{
public:
    Enemy_Blue_5(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    void skill();
    std::vector<Bullet*>* shoot2();
private:
    bool first_time;
};
#endif // ENEMY_BLUE_5_H
