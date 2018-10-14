#include "enemy_temp.h"

Enemy_Temp::Enemy_Temp(shoot_func shoot_func_ptr, Character* player, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy(":/res/bullet_black.png",0,0,0,0,player,1,1,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable), shoot_func_ptr(shoot_func_ptr)
{
    this->attackable=false;
    this->setInvulnerable();
}

std::vector<Bullet*>* Enemy_Temp::shoot() {
    return (this->*shoot_func_ptr)();
}

std::vector<Bullet*>* Enemy_Temp::enemy_4_pink_shoot() {
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%20==0) {
        double bullet_v, bullet_a, bullet_count, cos, sin, t;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a and count
        t = (shoot_timer-shoot_cd)/20;
        bullet_v = 0.5;
        bullet_a = 0.0005+t*0.00001;
        bullet_count = 4;
        if(shoot_timer==shoot_cd) angle=angleofvector(player->getX()-x,player->getY()-y);
        //shoot
        for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
            cos = std::cos(angle-i*M_PI/(bullet_count/2)-t*M_PI/(bullet_count*3));
            sin = std::sin(angle-i*M_PI/(bullet_count/2)-t*M_PI/(bullet_count*3));
            new_bullet = new Bullet(QString(":/res/bullet_pink.png"),16,x,y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        if(shoot_timer==shoot_cd+280) killItself();
        return new_bullets;
    }
    return nullptr;
}