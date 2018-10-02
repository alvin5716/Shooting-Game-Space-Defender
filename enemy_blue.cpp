#include "enemy_blue.h"
#include "laser.h"
#include <QDebug>
#include <cmath>

Enemy_Blue::Enemy_Blue(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    point=10;
    boss=true;
    invulnerable=true;
    connect(this,SIGNAL(useSkill(QString)),this,SIGNAL(killItsBullets()));
    connect(this,SIGNAL(deadSignal()),this,SIGNAL(killItsBullets()));
    connect(this,SIGNAL(deadSignal()),this,SLOT(deadSet()));
}
void Enemy_Blue::deadSet() {
    emit deadSignal(x,y);
    emit useSkill("");
}
std::vector<Bullet*>* Enemy_Blue::shoot() {
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%40==0) {
        double bullet_v, bullet_a, cos, sin;
        int bullet_radius, bullet_count, t;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a
        t = (shoot_timer-shoot_cd)/40;
        bullet_v = (t==3)?2:0.2;
        bullet_a = (t==3)?0.01:0.01+t*0.003;
        bullet_count = (t==3)?42:20;
        bullet_radius = (t==3)?9:18;
        if(shoot_timer==shoot_cd) {
            invulnerable=false;
            angle=angleofvector(player->getX()-x,player->getY()-y);
        }
        //shoot
        for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
            cos = std::cos(angle+i*M_PI/(bullet_count/2)+((t==3)?M_PI/bullet_count:0));
            sin = std::sin(angle+i*M_PI/(bullet_count/2)+((t==3)?M_PI/bullet_count:0));
            new_bullet = new Bullet(QString(":/res/bullet_blue.png"),bullet_radius,x,y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        if(shoot_timer==shoot_cd+120) {
            shoot_timer = 0;
        }
        return new_bullets;
    }
    return nullptr;
}
