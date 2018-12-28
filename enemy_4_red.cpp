#include "enemy_4_red.h"
#include "game.h"

Enemy_4_Red::Enemy_4_Red(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool single_attack)
:Enemy_4(QString(":/res/enemy/4/red.png"),200,153,std::round(3.902*radius),3*radius,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,true,true)
    ,single_attack(single_attack),leaving(false)
{
    move_speed=3.2;
    invulnerable = true;
}

void Enemy_4_Red::skill() {
    if(prep_timer>0) {
        --prep_timer;
        return;
    }
    if(!invulnerable_after_init) invulnerable = false;
    if(shoot_timer<move_time) {
        double angle = angleofvector(player->getX()-x,player->getY()-y);
        setSpeed(move_speed*std::cos(angle),move_speed*std::sin(angle));
    } else if(shoot_timer==move_time){
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
        new_bullet = new Bullet(QString(":/res/bullet/1/red.png"),12,x,y,bullet_v*cos,bullet_v*sin);
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
