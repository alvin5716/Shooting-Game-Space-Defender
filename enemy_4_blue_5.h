#ifndef ENEMY_4_BLUE_5_H
#define ENEMY_4_BLUE_5_H

#include "enemy_4_blue.h"

class Enemy_4_Blue_5 :public Enemy_4_Blue
{
public:
    Enemy_4_Blue_5(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    void skill() override;
    std::vector<Bullet*>* shoot2() override;
    static QPoint getCorpsePos();
    Effect* disappear() override;
private:
    static QPoint corpse_pos;
    static const int max_distance = 160;
    int prep_count;
};

#endif // ENEMY_4_BLUE_5_H
