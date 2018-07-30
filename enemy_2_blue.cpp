#include "enemy_2_blue.h"
#include "laser.h"
#include <QDebug>
#include <cmath>
#include "bullet_rotate.h"

Enemy_2_Blue::Enemy_2_Blue(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    invulnerable=true;
    point=15;
    boss=true;
    connect(this,SIGNAL(useSkill(QString)),this,SIGNAL(killItsBullets()));
    connect(this,SIGNAL(deadSignal()),this,SIGNAL(killItsBullets()));
    connect(this,SIGNAL(deadSignal()),this,SLOT(deadSet()));
    fadein(250);
}
void Enemy_2_Blue::deadSet() {
    emit deadSignal(x,y);
    emit useSkill("");
}
std::vector<Bullet*>* Enemy_2_Blue::shoot() {
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%200==0) {
        double bullet_v, bullet_count, cos, sin;
        int t, bullet_radius;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a and count
        t = (shoot_timer-shoot_cd)/200;
        bullet_v = 0.8;
        bullet_count = 30;
        bullet_radius = 8;
        if(shoot_timer==shoot_cd) {
            invulnerable=false;
            angle=qrand()%360*M_PI/360;
        }
        //shoot
        for(int j=0;j<2;++j) {
            for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
                cos = std::cos(angle+i*M_PI/(bullet_count/2)+t*M_PI/30);
                sin = std::sin(angle+i*M_PI/(bullet_count/2)+t*M_PI/30);
                new_bullet = new Bullet_Rotate(QString(":/res/bullet_2_blue.png"),x,y,0.001,j==0,bullet_radius,x,y+radius,bullet_v*cos,bullet_v*sin);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
        if(shoot_timer==shoot_cd+200) {
            shoot_timer = 0;
        }
        return new_bullets;
    }
    return NULL;
}
QString Enemy_2_Blue::rainbowBullet(int i) const{
    QString str;
    switch(i) {
    case -3:
        str=":/res/bullet_2_pink.png";
        break;
    case -2:
        str=":/res/bullet_2_red.png";
        break;
    case -1:
        str=":/res/bullet_2_yellow.png";
        break;
    case 0:
        str=":/res/bullet_2_green.png";
        break;
    case 1:
        str=":/res/bullet_2_blue.png";
        break;
    case 2:
        str=":/res/bullet_2_indigo.png";
        break;
    case 3:
        str=":/res/bullet_2_purple.png";
        break;
    }
    return str;
}
QString Enemy_2_Blue::rainbowLaser(int i) const{
    QString str;
    switch(i) {
    case -3:
        str=":/res/laser_pink.png";
        break;
    case -2:
        str=":/res/laser_red.png";
        break;
    case -1:
        str=":/res/laser_yellow.png";
        break;
    case 0:
        str=":/res/laser_green.png";
        break;
    case 1:
        str=":/res/laser_blue.png";
        break;
    case 2:
        str=":/res/laser_indigo.png";
        break;
    case 3:
        str=":/res/laser_purple.png";
        break;
    }
    return str;
}
