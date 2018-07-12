#include "enemy_2_blue.h"
#include "laser.h"
#include <QDebug>
#include <cmath>
#include "bullet_terminal.h"

Enemy_2_Blue::Enemy_2_Blue(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    mode=false;
    point=10;
    boss=true;
    connect(this,SIGNAL(useSkill(QString)),this,SIGNAL(killItsBullets()));
    connect(this,SIGNAL(deadSignal()),this,SIGNAL(killItsBullets()));
    connect(this,SIGNAL(deadSignal()),this,SLOT(deadSet()));
}
void Enemy_2_Blue::deadSet() {
    emit deadSignal(x,y);
    emit useSkill("");
}
std::vector<Bullet*>* Enemy_2_Blue::shoot() {
    static double tana=0, cosa=0, sina=0;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%8==0) {
        double bullet_v, bullet_a, cosb, sinb, cos, sin;
        int bullet_radius, t;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a
        t = (shoot_timer-shoot_cd)/8;
        bullet_radius = 9;
        if(shoot_timer==shoot_cd) {
            tana = (y-player->getY()) / (x-player->getX());
            if(std::isinf(tana)) cosa = 0;
            else cosa = ((player->getX()>x)?1:-1)/ sqrt(tana*tana+1);
            if(std::isinf(tana)) sina = 1;
            else sina = tana*cosa;
        }
        //shoot
        for(int i=-3;i<=3;++i) {
            bullet_v = mode?1:1.5;
            bullet_a = mode?(i+2)*0.0006:i*0.0005;
            cosb = std::cos(i*M_PI/24+(mode?-1:1)*t*M_PI/15.7);
            sinb = std::sin(i*M_PI/24+(mode?-1:1)*t*M_PI/15.7);
            cos = cosa*cosb-sina*sinb;
            sin = sina*cosb+cosa*sinb;
            new_bullet = new Bullet(rainbowBullet(i),bullet_radius,x,y+radius*3/4,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullet->fadein(1600);
            new_bullets->push_back(new_bullet);
        }
        if(shoot_timer==shoot_cd+800) {
            shoot_timer = 0;
            mode=!mode;
        }
        return new_bullets;
    }
    return NULL;
}
QString Enemy_2_Blue::rainbowBullet(int i) {
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
