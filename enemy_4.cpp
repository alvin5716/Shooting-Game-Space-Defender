#include "enemy_4.h"
#include <QDebug>

int Enemy_4::strategy_count[] = {0};

Enemy_4::Enemy_4(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    setCanBeMirrored();
    this->strategy = Enemy4strategy::back;
    ++strategy_count[Enemy4strategy::back];
    this->speed = 1.2;
}

void Enemy_4::skill() {
    switch (this->strategy) {
    case Enemy4strategy::back:
        backStratery();
        break;
    case Enemy4strategy::front:
        frontStratery();
        break;
    case Enemy4strategy::kite:
        kiteStratery();
        break;
    default:
        break;
    }
}

void Enemy_4::backStratery() {
    double speed_angle;
    if(y>=player->getY()) speed_angle = angleofvector(player->getX()-x,player->getY()-y+radius/2);
    else {
        speed_angle=M_PI/2;
    }
    Character::setSpeed(speed*std::cos(speed_angle),speed*std::sin(speed_angle));
}

void Enemy_4::frontStratery() {

}
void Enemy_4::kiteStratery() {

}

void Enemy_4::setStrategy(Enemy4strategy strategy) {
    //remove count
    --strategy_count[this->strategy];
    //set
    this->strategy = strategy;
    //add count
    ++strategy_count[this->strategy];
}

int Enemy_4::getStrategyCount(Enemy4strategy strategy) const{
    return strategy_count[strategy];
}

void Enemy_4::setSpeed(double speed) {
    this->speed=speed;
}

void Enemy_4::setInvulnerable() {
    Character::setInvulnerable();
    setStrategy(Enemy4strategy::front);
}

Enemy_4::~Enemy_4() {
    --strategy_count[this->strategy];
}
