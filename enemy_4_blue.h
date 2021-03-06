#ifndef ENEMY_4_BLUE_H
#define ENEMY_4_BLUE_H

#include "enemy_4.h"

class Enemy_4_Blue :public Enemy_4
{
    Q_OBJECT
public:
    Enemy_4_Blue(Player* player, int bossSkillHP, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    std::vector<Bullet*>* shoot() override;
    void skill() override;
public slots:
    void deadSet();
    void small_enemy_died();
protected:
    void setSummonCD(int summon_cd);
    int summon_timer;
    Enemy* small_enemy;
private:
    int summon_cd;
    double angle[2];
    double attack_x, attack_y;
};

#endif // ENEMY_4_BLUE_H
