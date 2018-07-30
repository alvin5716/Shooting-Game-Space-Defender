#include "enemy_red.h"
#include "bullet_terminal.h"
#include <cmath>

Enemy_Red::Enemy_Red(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
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
                new_bullet = new Bullet_Terminal(QString(":/res/bullet_red.png"),8,3.5,x,y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
        return new_bullets;
    }
    return NULL;
}
