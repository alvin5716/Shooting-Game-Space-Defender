#include "enemy_3_pink.h"

Enemy_3_Pink::Enemy_3_Pink(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_3(QString(":/res/enemy/3/pink.png"),54,43,std::round(radius*2.512),radius*2,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    mode=false;
    point=4;
}
std::vector<Bullet*>* Enemy_3_Pink::shoot() {
    const int interval=30;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        double bullet_v, bullet_a, bullet_v_2, bullet_a_2, cos, sin;
        int t, bullet_count;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a and count
        t = (shoot_timer-shoot_cd)/interval;
        bullet_v = 2.5;
        bullet_a = -0.018;
        bullet_v_2 = 1;
        bullet_a_2 = 0.015-t*0.0012;
        bullet_count = 14;
        if(shoot_timer==shoot_cd) angle = angleofvector(player->getX()-x,player->getY()-y);
        //shoot
        for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
            double shoot_angle = angle+(i+(mode?0.05:0.95))*M_PI/(bullet_count/2);
            cos = std::cos(shoot_angle);
            sin = std::sin(shoot_angle);
            new_bullet = new Bullet(QString(":/res/bullet/1/pink.png"),12,x,y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            cos = std::cos(shoot_angle+(mode?M_PI/5:-M_PI/5));
            sin = std::sin(shoot_angle+(mode?M_PI/5:-M_PI/5));
            new_bullet->addTimeData(175,bullet_v_2*cos,bullet_v_2*sin,bullet_a_2*cos,bullet_a_2*sin);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        if(shoot_timer==shoot_cd+interval*4) {
            shoot_timer = 0;
            mode=!mode;
        }
        return new_bullets;
    }
    return nullptr;
}
