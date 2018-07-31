#include "enemy_3_red.h"
#include "bullet_sin.h"

Enemy_3_Red::Enemy_3_Red(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_3(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    point=3;
}
std::vector<Bullet*>* Enemy_3_Red::shoot() {
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%12==0) {
        double bullet_v, bullet_a, angle;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a and count
        bullet_v = 1.5;
        bullet_a = 0.008;
        if(shoot_timer==shoot_cd) {
            angle = angleofvector(player->getX()-x,player->getY()-y);
            sin = std::sin(angle);
            cos = std::cos(angle);
        }
        //shoot
        for(int i=0;i<2;++i) {
            new_bullet = new Bullet_Sin(QString(":/res/bullet_red.png"),80,30,9,x,y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin,i==0);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        if(shoot_timer==shoot_cd+108) shoot_timer = 0;
        return new_bullets;
    }
    return NULL;
}
