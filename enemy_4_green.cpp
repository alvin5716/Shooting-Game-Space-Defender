#include "enemy_4_green.h"

Enemy_4_Green::Enemy_4_Green(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_4(QString(":/res/enemy16.png"),199,153,std::round(3.902*radius),3*radius,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    move_speed = 0.85;
    setInvulnerable();
}

void Enemy_4_Green::skill() {
    if(prep_timer>0) {
        --prep_timer;
        return;
    }
    setVulnerable();
    double angle = angleofvector(player->getX()-x,player->getY()-y);
    setSpeed(move_speed*std::cos(angle),move_speed*std::sin(angle));
}

std::vector<Bullet*>* Enemy_4_Green::shoot() {
    if(prep_timer>0) return nullptr;
    if(shoot_timer>=shoot_cd) {
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        double sin, cos, angle = angleofvector(player->getX()-x,player->getY()-y), bullet_v=move_speed+0.5;
        for(int i=0;i<2;++i) {
            sin = std::sin(angle+(i?1:-1)*2*M_PI/20);
            cos = std::cos(angle+(i?1:-1)*2*M_PI/20);
            new_bullet = new Bullet(QString(":/res/bullet_green.png"),12,x,y,bullet_v*cos,bullet_v*sin);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        shoot_timer=0;
        return new_bullets;
    }
    return nullptr;
}
