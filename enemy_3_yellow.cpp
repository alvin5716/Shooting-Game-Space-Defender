#include "enemy_3_yellow.h"
#include "bullet_sin.h"

Enemy_3_Yellow::Enemy_3_Yellow(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_3(QString(":/res/enemy/3/yellow.png"),54,43,std::round(radius*2.512),radius*2,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    point=4;
}
std::vector<Bullet*>* Enemy_3_Yellow::shoot() {
    if(shoot_timer>=shoot_cd) {
        double bullet_v, bullet_a, bullet_count, cos, sin;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a and count
        bullet_count = 14;
        angle = angleofvector(player->getX()-x,player->getY()-y);
        //shoot
        for(int j=0;j<3;++j) {
            bullet_v = 1.6+0.36*j;
            bullet_a = 0.004+0.0002*j;
            for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
                cos = std::cos(angle+i*M_PI/(bullet_count/2));
                sin = std::sin(angle+i*M_PI/(bullet_count/2));
                if(j==1) new_bullet = new Bullet(QString(":/res/bullet/1/yellow.png"),12,x,y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                else new_bullet = new Bullet_Sin(QString(":/res/bullet/1/yellow.png"),125,35,12,x,y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin,j==0);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
        shoot_timer = 0;
        return new_bullets;
    }
    return nullptr;
}
