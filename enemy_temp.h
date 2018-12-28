#ifndef ENEMY_TEMP_H
#define ENEMY_TEMP_H

#include "enemy.h"

class Enemy_Temp: public Enemy
{
public:
    typedef std::vector<Bullet*>* (Enemy_Temp::*shoot_func)(void);
    Enemy_Temp(Enemy* real_shooter, shoot_func shoot_func_ptr, Player* player, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    std::vector<Bullet*>* shoot() override;
    Effect* disappear() override;
    // shoot method
    std::vector<Bullet*>* enemy_4_pink_shoot();
    std::vector<Bullet*>* enemy_4_pink_shoot_2();
    std::vector<Bullet*>* enemy_4_blue_4_shoot();
private:
    shoot_func shoot_func_ptr;
    double angle;
    Enemy* real_shooter;
};

#endif // ENEMY_TEMP_H
