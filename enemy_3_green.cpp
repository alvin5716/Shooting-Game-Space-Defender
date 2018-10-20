#include "enemy_3_green.h"
#include "bullet_sin.h"

Enemy_3_Green::Enemy_3_Green(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_3(QString(":/res/enemy11.png"),54,43,std::round(radius*2.512),radius*2,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{

}
std::vector<Bullet*>* Enemy_3_Green::shoot() {
    const int interval=18;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        double bullet_v, bullet_a, bullet_count, cos, sin, t;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a and count
        t = (shoot_timer-shoot_cd)/interval;
        bullet_v = 1.2;
        bullet_a = 0.001+t*0.00005;
        bullet_count = 8;
        if(shoot_timer==shoot_cd) angle = angleofvector(player->getX()-x,player->getY()-y);
        //shoot
        for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
            cos = std::cos(angle+i*M_PI/(bullet_count/2));
            sin = std::sin(angle+i*M_PI/(bullet_count/2));
            new_bullet = new Bullet_Sin(QString(":/res/bullet_green.png"),200,15,12,x,y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        if(shoot_timer==shoot_cd+interval*3) shoot_timer = 0;
        return new_bullets;
    }
    return nullptr;
}
