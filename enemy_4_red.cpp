#include "enemy_4_red.h"

Enemy_4_Red::Enemy_4_Red(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
:Enemy_4(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    move_speed=1.6;
    bounceable=true;
    setInvulnerable();
}

void Enemy_4_Red::skill() {
    if(prep_timer>0) {
        --prep_timer;
        return;
    }
    setVulnerable();
    if(shoot_timer<shoot_cd) {
        double angle = angleofvector(player->getX()-x,player->getY()-y);
        setSpeed(move_speed*std::cos(angle),move_speed*std::sin(angle));
    } else if(shoot_timer==shoot_cd){
        double angle = angleofvector(player->getX()-x,player->getY()-y);
        constexpr int back=300;
        moveTo(x-back*std::cos(angle),y-back*std::sin(angle));
    }
}

std::vector<Bullet*>* Enemy_4_Red::shoot() {
    if(prep_timer>0) return nullptr;
    constexpr int interval=10, shoot_count=15;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0 && (shoot_timer-shoot_cd)/interval<shoot_count) {
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        double sin, cos, angle = angleofvector(player->getX()-x,player->getY()-y), bullet_v=3.6;
        sin = std::sin(angle);
        cos = std::cos(angle);
        new_bullet = new Bullet(QString(":/res/bullet_red.png"),12,x,y,bullet_v*cos,bullet_v*sin);
        connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
        new_bullets->push_back(new_bullet);
        return new_bullets;
    }
    if(shoot_timer>=shoot_cd+interval*(shoot_count+15)) shoot_timer=0;
    return nullptr;
}
