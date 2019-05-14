#include "enemy_4_red.h"
#include "game.h"
#include <QDebug>

Enemy_4_Red::Enemy_4_Red(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool single_attack)
:Enemy_4(QString(":/res/enemy/4/red.png"),200,153,std::round(3.902*radius),3*radius,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,true,true)
    ,single_attack(single_attack),leaving(false)
{
    max_speed = 3.2;
    invulnerable = true;
}

void Enemy_4_Red::skill() {
    if(prep_timer>0) {
        --prep_timer;
        if(prep_timer==1) stopable = false;
        return;
    }
    if(!invulnerable_after_init) invulnerable = false;
    if(shoot_timer<move_time) {
        if(shoot_timer==0) stopable = false;
        double angle = angleofvector(player->getX()-x,player->getY()-y);
        setAcceleration(max_acceleration*std::cos(angle),max_acceleration*std::sin(angle));
        double speed = std::sqrt(std::pow(xv,2)+std::pow(yv,2));
        if(speed>max_speed) {
            xv *= max_speed / speed;
            yv *= max_speed / speed;
        }
        const int prepTime = 50;
        if(shoot_timer==move_time-prepTime) {
            PrepEffectInfo prepInfo(":/res/bullet/4/red.png",50,50);
            prepInfo.setTime(prepTime);
            prepInfo.setScale(2);
            prepInfo.setSound(false);
            prepEffect(prepInfo);
        }
    } else if(shoot_timer==move_time) {
        stopable = true;
        double angle = angleofvector(player->getX()-x,player->getY()-y);
        constexpr int back=300;
        moveTo(x-back*std::cos(angle),y-back*std::sin(angle));
    }
}

std::vector<Bullet*>* Enemy_4_Red::shoot() {
    if(prep_timer>0 || leaving) return nullptr;
    const int interval=5, shoot_count=15;
    if(shoot_timer>=move_time && (shoot_timer-move_time)%interval==0 && (shoot_timer-move_time)/interval<shoot_count) {
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        double sin, cos, angle = angleofvector(player->getX()-x,player->getY()-y), bullet_v=7.2;
        sin = std::sin(angle);
        cos = std::cos(angle);
        new_bullet = new Bullet(QString(":/res/bullet/4/red.png"),12,x,y,bullet_v*cos,bullet_v*sin);
        connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
        new_bullets->push_back(new_bullet);
        return new_bullets;
    }
    if(shoot_timer>=move_time+interval*shoot_count+shoot_cd) {
        if(single_attack) {
            leaving=true;
            bounceable=false;
            if(this->x<Game::FrameWidth-this->x) setSpeed(-3,0);
            else setSpeed(3,0);
        } else shoot_timer=0;
    }
    return nullptr;
}
