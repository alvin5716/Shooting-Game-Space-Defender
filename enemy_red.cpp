#include "enemy_red.h"
#include <cmath>

Enemy_Red::Enemy_Red(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy(QString(":/res/enemy/1/red.png"),40,37,std::round(radius*2.162),radius*2,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    point=3;
}

std::vector<Bullet*>* Enemy_Red::shoot() {
    if(shoot_timer>=shoot_cd) {
        shoot_timer = 0;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        double bullet_v, bullet_a, cos, sin, angle;
        angle=angleofvector(player->getX()-x,player->getY()-y);
        for(int i=-1;i<=1;++i) {
            for(int j=0;j<3;++j) {
                cos = std::cos(angle+i*M_PI/15);
                sin = std::sin(angle+i*M_PI/15);
                bullet_v = 0.25;
                bullet_a = 0.01+0.001*j;
                new_bullet = new Bullet(QString(":/res/bullet/1/red.png"),8,x,y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                new_bullet->setVTerminal(3.5);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
        return new_bullets;
    }
    return nullptr;
}
