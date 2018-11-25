#ifndef ENEMY_4_BLUE_1_H
#define ENEMY_4_BLUE_1_H

#include "enemy_4_blue.h"
#include "spreadrng.h"

class Enemy_4_Blue_1 :public Enemy_4_Blue
{
public:
    Enemy_4_Blue_1(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    QPoint bulletPos(int bulletNum) const;
    QString rainbowBullet(int i) const;
    void skill() override;
    std::vector<Bullet*>* shoot2() override;
private:
    static constexpr int bullet_count = 30;
    bool first;
    SpreadRNG* rng;
    Bullet* boat_bullets[bullet_count];
};

#endif // ENEMY_4_BLUE_1_H
