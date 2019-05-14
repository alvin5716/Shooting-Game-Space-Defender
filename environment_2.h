#ifndef ENVIRONMENT_2_H
#define ENVIRONMENT_2_H

#include "environment.h"
#include "spreadrng.h"

class Environment_2 : public Environment
{
public:
    Environment_2(Player *player, int shoot_cd, int lifetime=750);
    std::vector<Bullet*>* shoot() override;
private:
    SpreadRNG* rng;
};

#endif // ENVIRONMENT_2_H
