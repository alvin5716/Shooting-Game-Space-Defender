#ifndef ENEMY_2_BLUE_H
#define ENEMY_2_BLUE_H

#include "game.h"
#include "enemy.h"
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class Enemy_2_Blue : public Enemy
{
    Q_OBJECT
public:
    Enemy_2_Blue(Player* player, int bossSkillHP, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    std::vector<Bullet*>* shoot();
public slots:
    void deadSet();
protected:
    QString rainbowBullet(int i) const;
    QString rainbowLaser(int i) const;
private:
    double angle;
};

#endif // ENEMY_2_BLUE_H
