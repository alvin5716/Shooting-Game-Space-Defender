#ifndef ENEMY_3_BLUE_3_H
#define ENEMY_3_BLUE_3_H

#include "enemy_3_blue.h"
#include "bullet_time.h"
#include <QString>
#include <vector>

class Enemy_3_Blue_3 : public Enemy_3_Blue
{
public:
    Enemy_3_Blue_3(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    void skill();
    std::vector<Bullet*>* shoot2();
private:
    bool use_laser;
    short shoot_count_yellow, shoot_count;
    Bullet_Time* center;
};

#endif // ENEMY_3_BLUE_3_H
