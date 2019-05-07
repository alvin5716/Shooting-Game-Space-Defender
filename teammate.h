#ifndef TEAMMATE_H
#define TEAMMATE_H

#include "enemy.h"
#include "bullet.h"

class Teammate : public Enemy
{
public:
    Teammate(QString img, int img_w, int img_h, int show_w, int show_h, Player* player, int shoot_cd, double x=0, double y=0);
    std::vector<Bullet*>* shoot() override;
    void move() override;
    void setShooting(bool shooting=true);
    bool isShooting();
    void sendNearestEnemyPos(bool hasEnemy, QPoint nearestEnemyPos = QPoint(-1,-1), QPoint speed = QPoint(-1,-1));
private:
    bool hasEnemy, shooting;
    QPoint aim_pos, aim_speed;
};

#endif // TEAMMATE_H
