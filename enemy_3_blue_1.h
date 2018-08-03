#ifndef ENEMY_3_BLUE_1_H
#define ENEMY_3_BLUE_1_H

#include "enemy_3_blue.h"
#include "bullet_wall.h"
#include <QString>
#include <vector>

class Enemy_3_Blue_1 : public Enemy_3_Blue
{
    Q_OBJECT
public:
    Enemy_3_Blue_1(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    void skill();
    std::vector<Bullet*>* shoot2();
public slots:
    void shootSmallBullet();
private:
    double angle;
    Bullet_Wall* magicstone;
    bool shoot_small_bullets, mode;
};

#endif // ENEMY_3_BLUE_1_H
