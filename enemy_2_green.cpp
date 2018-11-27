#include "enemy_2_green.h"
#include <cmath>

Enemy_2_Green::Enemy_2_Green(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy(QString(":/res/enemy/2/green.png"),35,35,radius*2,radius*2,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{

}

std::vector<Bullet*>* Enemy_2_Green::shoot() {
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%11==0) {
        double bullet_v, bullet_a, bullet_count, cos, sin, t;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a and count
        t = (shoot_timer-shoot_cd)/11;
        bullet_v = 1.2;
        bullet_a = 0.001+t*0.00005;
        bullet_count = 16;
        if(shoot_timer==shoot_cd) angle = angleofvector(player->getX()-x,player->getY()-y);
        //shoot
        for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
            cos = std::cos(angle+i*M_PI/(bullet_count/2));
            sin = std::sin(angle+i*M_PI/(bullet_count/2));
            new_bullet = new Bullet(QString(":/res/bullet/2/green.png"),8,x,y+radius*3/4,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        if(shoot_timer==shoot_cd+55) shoot_timer = 0;
        return new_bullets;
    }
    return nullptr;
}
