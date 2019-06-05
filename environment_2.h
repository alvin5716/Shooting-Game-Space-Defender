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
    double bias_k;
    double bias_i;
    int r;
    int center_x, center_y;
};

#endif // ENVIRONMENT_2_H
