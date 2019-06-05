#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "enemy_temp.h"

class Environment : public Enemy
{
public:
    Environment(const QString& img, Player* player, int shoot_cd, int lifetime=750);
    Environment(int pixmap, Player* player, int shoot_cd, int lifetime=750);
public slots:
    void move() override;
    void img_move() override;
protected:
    int lifetimer;
};

#endif // ENVIROMENT_H
