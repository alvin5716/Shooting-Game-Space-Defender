#ifndef ENVIRONMENT_1_H
#define ENVIRONMENT_1_H

#include "environment.h"

class Environment_1 : public Environment
{
public:
    Environment_1(Player *player, int shoot_cd, int lifetime=750);
    std::vector<Bullet*>* shoot() override;
private:
    double angle;
};

#endif // ENVIRONMENT_1_H
