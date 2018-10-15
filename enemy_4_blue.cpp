#include "enemy_4_blue.h"

Enemy_4_Blue::Enemy_4_Blue(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_4(QString(":/res/enemy20.png"),199,153,std::round(3.902*radius),3*radius,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    point=12;
    boss=true;
    invulnerable=true;
    connect(this,SIGNAL(useSkill(QString)),this,SIGNAL(killItsBullets()));
    connect(this,SIGNAL(deadSignal()),this,SIGNAL(killItsBullets()));
    connect(this,SIGNAL(deadSignal()),this,SLOT(deadSet()));
}

void Enemy_4_Blue::deadSet() {
    emit deadSignal(x,y);
    emit useSkill("");
}

std::vector<Bullet*>* Enemy_4_Blue::shoot() {
    return nullptr;
}
