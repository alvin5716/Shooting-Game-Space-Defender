#ifndef ENVIRONMENT_3_H
#define ENVIRONMENT_3_H

#include "environment.h"
#include "spreadrng.h"

class Bullet_Seed;

class Environment_3 : public Environment
{
public:
    Environment_3(Player *player, int shoot_cd, int lifetime=750);
    std::vector<Bullet*>* shoot();
    ~Environment_3();
private:
    SpreadRNG* rng_x, *rng_cd;
    std::vector<Bullet_Seed*> seeds;
};

class Bullet_Seed : public QObject{
    Q_OBJECT
public:
    Bullet_Seed(Bullet* bullet) : bullet(bullet), triggered(false), died_early(false), pos(-1,-1) {
        connect(bullet, SIGNAL(triggered()), this, SLOT(grow()));
        connect(bullet, SIGNAL(deadSignal()), this, SLOT(killed()));
    }
    Bullet* bullet;
    bool triggered;
    bool died_early;
    QPoint pos;
public slots:
    void grow() {
        pos.setX(bullet->getX());
        pos.setY(bullet->getY());
        triggered = true;
        bullet->killItself();
    }
    void killed() {
        died_early = true;
    }
};

#endif // ENVIRONMENT_3_H
