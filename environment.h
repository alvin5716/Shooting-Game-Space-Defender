#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "enemy_temp.h"

class Environment : public Enemy_Temp
{
public:
    Environment(Player* player, int shoot_cd, int lifetime=750);
public slots:
    void move() override;
protected:
    int lifetimer;
};

#endif // ENVIROMENT_H
