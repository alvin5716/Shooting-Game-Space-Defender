#include "enemy_2_red.h"

Enemy_2_Red::Enemy_2_Red(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy(QString(":/res/enemy/2/red.png"),35,35,radius*2,radius*2,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    point=3;
}

std::vector<Bullet*>* Enemy_2_Red::shoot() {
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%5==0) {
        double bullet_v, bullet_a, cos, sin;
        int t;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a and count
        t = (shoot_timer-shoot_cd)/5;
        bullet_v = 2;
        bullet_a = 0;
        if(shoot_timer==shoot_cd) angle=angleofvector(player->getX()-x,player->getY()-y);
        //shoot
        cos = std::cos(angle+((((t+3)/7%2==0)?((t+3)%7):7-((t+3)%7))-3)*M_PI/80);
        sin = std::sin(angle+((((t+3)/7%2==0)?((t+3)%7):7-((t+3)%7))-3)*M_PI/80);
        new_bullet = new Bullet(QString(":/res/bullet/2/red.png"),6,x,y+radius*3/4,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
        connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
        new_bullets->push_back(new_bullet);
        if(shoot_timer==shoot_cd+180) shoot_timer = 0;
        return new_bullets;
    }
    return nullptr;
}
