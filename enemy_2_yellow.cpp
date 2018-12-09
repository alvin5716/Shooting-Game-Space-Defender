#include "enemy_2_yellow.h"
#include <cmath>

Enemy_2_Yellow::Enemy_2_Yellow(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy(QString(":/res/enemy/2/yellow.png"),35,35,radius*2,radius*2,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    mode=true;
    point=3;
}

std::vector<Bullet*>* Enemy_2_Yellow::shoot() {
    if(shoot_timer==shoot_cd) {
        double bullet_v, bullet_a, cos, sin;
        int bullet_count, bullet_radius;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a and count
        bullet_v = 1.7;
        bullet_count = 48;
        angle=angleofvector(player->getX()-x,player->getY()-y);
        //shoot
        for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
            int k;
            if(mode)
                k = ((i+bullet_count/2)/3%2==0)?((i+bullet_count/2)%3):3-((i+bullet_count/2)%3);
            else
                k = ((i+3+bullet_count/2)/3%2==0)?((i+3+bullet_count/2)%3):3-((i+3+bullet_count/2)%3);
            bullet_radius = (k==3)?16:8;
            bullet_a = 0.0008+k*0.002;
            cos = std::cos(angle+i*M_PI/(bullet_count/2));
            sin = std::sin(angle+i*M_PI/(bullet_count/2));
            new_bullet = new Bullet(QString(":/res/bullet/2/yellow.png"),bullet_radius,x,y+radius*3/4,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        shoot_timer = 0;
        mode=!mode;
        return new_bullets;
    }
    return nullptr;
}
