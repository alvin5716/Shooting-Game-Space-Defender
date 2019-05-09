#include "environment_1.h"
#include "bullet_effect.h"

Environment_1::Environment_1(Player *player, int shoot_cd, int lifetime)
    :Environment(player,shoot_cd,lifetime)
{

}

std::vector<Bullet*>* Environment_1::shoot() {
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%13==0) {
        double bullet_v, bullet_a, bullet_count, cos, sin, t;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a and count
        t = (shoot_timer-shoot_cd)/13;
        bullet_v = 2.4;
        bullet_a = 0.004+t*0.0002;
        bullet_count = 20;
        if(shoot_timer==shoot_cd) angle=angleofvector(player->getX()-x,player->getY()-y);
        //shoot
        for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
            cos = std::cos(angle+i*M_PI/(bullet_count/2)+t*M_PI/(bullet_count*4));
            sin = std::sin(angle+i*M_PI/(bullet_count/2)+t*M_PI/(bullet_count*4));
            new_bullet = new Bullet_Effect(QString(":/res/bullet/5/white.png"),14,this->scene(),QString(":/res/bullet/5/yellow.png"),1.3,x,y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        if(shoot_timer==shoot_cd+26) shoot_timer = 0;
        return new_bullets;
    }
    return nullptr;
}
