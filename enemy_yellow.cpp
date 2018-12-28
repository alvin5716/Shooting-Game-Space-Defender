#include "enemy_yellow.h"
#include <cmath>
#include <QDebug>

Enemy_Yellow::Enemy_Yellow(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy(QString(":/res/enemy/1/yellow.png"),40,37,std::round(radius*2.162),radius*2,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    point=5;
}

std::vector<Bullet*>* Enemy_Yellow::shoot() {
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
            new_bullet = new Bullet(QString(":/res/bullet/1/yellow.png"),16,x,y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        if(shoot_timer==shoot_cd+26) shoot_timer = 0;
        return new_bullets;
    }
    return nullptr;
}
