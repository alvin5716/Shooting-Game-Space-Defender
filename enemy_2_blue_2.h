#ifndef ENEMY_2_BLUE_2_H
#define ENEMY_2_BLUE_2_H

#include "enemy_2_blue.h"
#include <QString>
#include <vector>
#include "bullet_bounce.h"

class Enemy_2_Blue_2 : public Enemy_2_Blue
{
public:
    Enemy_2_Blue_2(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    void skill();
    std::vector<Bullet*>* shoot2();
private:
    bool mode, aim_summoned;
    Bullet_Bounce* aim[7];
};

#endif // ENEMY_2_BLUE_2_H
