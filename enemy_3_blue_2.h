#ifndef ENEMY_3_BLUE_2_H
#define ENEMY_3_BLUE_2_H

#include "enemy_3_blue.h"
#include <QString>
#include <vector>

class Enemy_3_Blue_2 : public Enemy_3_Blue
{
public:
    Enemy_3_Blue_2(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    void skill();
    std::vector<Bullet*>* shoot2();
private:
    Effect* room;
    int room_radius_rate, omega_seed;
    static const int room_radius = 200;
    double angle, omega;
    bool mode, isBulletFaster, first_attack;
};

#endif // ENEMY_3_BLUE_2_H