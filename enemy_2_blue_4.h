#ifndef ENEMY_2_BLUE_4_H
#define ENEMY_2_BLUE_4_H

#include "enemy_2_blue.h"
#include <QString>
#include <vector>

class Enemy_2_Blue_4 : public Enemy_2_Blue
{
    Q_OBJECT
public:
    Enemy_2_Blue_4(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    void skill();
    std::vector<Bullet*>* shoot2();
signals:
    void killSpawnpoints();
private:
    int shoot_count;
    double bullet_angle;
    bool is_laser_used;
    Bullet *rainbowSpawnpoint[2], *stick[2];
};

#endif // ENEMY_2_BLUE_4_H
