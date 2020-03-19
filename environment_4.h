#ifndef ENVIRONMENT_4_H
#define ENVIRONMENT_4_H

#include "environment.h"

class Environment_4 : public Environment
{
public:
    Environment_4(Player *player, int shoot_cd, int lifetime=750);
    std::vector<Bullet*>* shoot() override;
private:
    double angle;
};

#endif // ENVIRONMENT_4_H
