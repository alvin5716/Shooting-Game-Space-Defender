#ifndef ENVIRONMENT_5_H
#define ENVIRONMENT_5_H

#include "environment.h"

class Environment_5 : public Environment
{
public:
    Environment_5(Player *player, int shoot_cd, int lifetime=750);
    std::vector<Bullet*>* shoot() override;
private:
    double angle;
    short laser_timer;
};

#endif // ENVIRONMENT_5_H
