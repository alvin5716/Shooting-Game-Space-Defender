#ifndef ENEMY_4_H
#define ENEMY_4_H

#include "enemy.h"
#include <QString>

class Enemy_4: public Enemy
{
public:
    Enemy_4(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    enum Enemy4strategy{back, front, kite};
    void skill() override;
    void setStrategy(Enemy4strategy strategy);
    int getStrategyCount(Enemy4strategy strategy) const;
    void setSpeed(double speed);
    void setInvulnerable() override;
    static int strategy_count[3];
    ~Enemy_4();
private:
    void backStratery();
    void frontStratery();
    void kiteStratery();
    Enemy4strategy strategy;
    double speed;
};

#endif // ENEMY_4_H
