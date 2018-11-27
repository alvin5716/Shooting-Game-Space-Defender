#include "enemy_4_pink.h"
#include "enemy_temp.h"
#include "game.h"

Enemy_4_Pink::Enemy_4_Pink(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool stopable)
:Enemy_4(QString(":/res/enemy/4/pink.png"),200,153,std::round(3.902*radius),3*radius,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,true,stopable)
{
    move_speed=1.6;
    this->shoot_cd_init=shoot_cd_init;
}

void Enemy_4_Pink::skill() {
    if(already_enter) {
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
    Character::attacked();
    if(health<=0) {
        Enemy* new_enemy = new Enemy_Temp(nullptr,Enemy_Temp::enemy_4_pink_shoot_2,player,shoot_cd,shoot_cd_init,x,y);
        emit summonEnemy(new_enemy);
    }
}
