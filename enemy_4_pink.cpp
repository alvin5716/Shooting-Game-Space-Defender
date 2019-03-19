#include "enemy_4_pink.h"
#include "enemy_temp.h"
#include "game.h"

Enemy_4_Pink::Enemy_4_Pink(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool stopable)
:Enemy_4(QString(":/res/enemy/4/pink.png"),200,153,std::round(3.902*radius),3*radius,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,true,stopable)
{
    move_speed=3.2;
    this->shoot_cd_init=shoot_cd_init;
}

void Enemy_4_Pink::skill() {
    if(already_enter && !dead) {
        if(x<0+radius || x>Game::FrameWidth-radius || y<0+radius || y>Game::FrameHeight-radius) {
            setAcceleration(0,0);
            setSpeed(0,0);
            Enemy* new_enemy = new Enemy_Temp(nullptr,Enemy_Temp::enemy_4_pink_shoot,player,shoot_cd,shoot_cd_init,x,y);
            emit summonEnemy(new_enemy);
            setDisappearTime(1200);
            killItself();
        }
    }
}

void Enemy_4_Pink::attacked() {
    Enemy_4::attacked();
    if(health<=0) {
        Enemy* new_enemy = new Enemy_Temp(nullptr,Enemy_Temp::enemy_4_pink_shoot_2,player,shoot_cd,shoot_cd_init,x,y);
        emit summonEnemy(new_enemy);
    }
}

void Enemy_4_Pink::setInvulnerable() {
    Character::setInvulnerable();
}
