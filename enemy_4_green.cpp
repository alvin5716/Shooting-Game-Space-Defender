#include "enemy_4_green.h"
#include <QDebug>

Enemy_4_Green::Enemy_4_Green(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable)
    :Enemy_4(QString(":/res/enemy/4/green.png"),200,153,std::round(3.902*radius),3*radius,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,true)
{
    max_speed = 1.7;
    invulnerable = true;
}

void Enemy_4_Green::skill() {
    if(prep_timer>0) {
        --prep_timer;
        return;
    }
    stopable = false;
    if(!invulnerable_after_init) invulnerable = false;
    double angle = angleofvector(player->getX()-x,player->getY()-y);
    setAcceleration(max_acceleration*std::cos(angle),max_acceleration*std::sin(angle));
    double speed = std::sqrt(std::pow(xv,2)+std::pow(yv,2));
    if(speed>max_speed) {
        xv *= max_speed / speed;
        yv *= max_speed / speed;
    }
}

std::vector<Bullet*>* Enemy_4_Green::shoot() {
    if(prep_timer>0) return nullptr;
    if(shoot_timer>=shoot_cd) {
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        double sin, cos, angle = angleofvector(player->getX()-x,player->getY()-y), bullet_v=max_speed+1;
        for(int i=0;i<2;++i) {
            sin = std::sin(angle+(i?1:-1)*2*M_PI/20);
            cos = std::cos(angle+(i?1:-1)*2*M_PI/20);
            new_bullet = new Bullet(QString(":/res/bullet/4/green.png"),12,x,y,bullet_v*cos,bullet_v*sin);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        shoot_timer=0;
        return new_bullets;
    }
    return nullptr;
}
