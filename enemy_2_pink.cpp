#include "enemy_2_pink.h"
#include "bullet_rotate.h"

Enemy_2_Pink::Enemy_2_Pink(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    mode=false;
}

std::vector<Bullet*>* Enemy_2_Pink::shoot() {
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%25==0) {
        double bullet_v, bullet_count, cosb, sinb, cos, sin;
        int t, bullet_radius;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a and count
        t = (shoot_timer-shoot_cd)/25;
        bullet_v = 0.8;
        bullet_count = 4;
        if(shoot_timer==shoot_cd) sincostoxy(sina,cosa,player->getX(),player->getY());
        //shoot
        for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
            bullet_radius = (t==0)?18:6;
            cosb = std::cos(i*M_PI/(bullet_count/2)+(mode?-1:1)*t*M_PI/30);
            sinb = std::sin(i*M_PI/(bullet_count/2)+(mode?-1:1)*t*M_PI/30);
            cos = cosa*cosb-sina*sinb;
            sin = sina*cosb+cosa*sinb;
            new_bullet = new Bullet_Rotate(QString(":/res/bullet_2_pink.png"),x,y+radius*3/4,0.01,mode,bullet_radius,x,y+radius*3/4,bullet_v*cos,bullet_v*sin);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        if(shoot_timer==shoot_cd+300) {
            shoot_timer = 0;
            mode=!mode;
        }
        return new_bullets;
    }
    return NULL;
}
