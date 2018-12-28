#include "enemy_2_blue_5.h"
#include "laser.h"
#include <QDebug>

Enemy_2_Blue_5::Enemy_2_Blue_5(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_2_Blue(player,170,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    mode=false;
}
void Enemy_2_Blue_5::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy/2/blue_2.png";
        shoot_timer = -60;
        shoot_cd = 175;
        skill_timer = -100;
        emit useSkill("七彩霓虹燈");
    },
    [this](){
        //skill
        if(skill_timer==0) moveTo(Game::FrameWidth/2,300,120);
        //skill timer
        if(skill_timer<=0) ++skill_timer;
    });
}
std::vector<Bullet*>* Enemy_2_Blue_5::shoot2() {
    if(shoot_timer==shoot_cd || shoot_timer==2*shoot_cd) {
        double bullet_v, cos, sin, phi;
        int bullet_radius;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet, *new_laser;
        //bullet v, a
        bullet_radius = 8;
        //shoot
        if(shoot_timer==shoot_cd) {
            angle = angleofvector(player->getX()-x,player->getY()-y);
            invulnerable=false;
            for(int k=-1;k<=1;++k) {
                for(int j=0;j<7;++j) {
                    for(int i=-3;i<=3;++i) {
                        bullet_v = 0.3+0.5*j;
                        cos = std::cos(angle+i*2*M_PI/7+(mode?M_PI/7:0)+k*M_PI/14);
                        sin = std::sin(angle+i*2*M_PI/7+(mode?M_PI/7:0)+k*M_PI/14);
                        new_bullet = new Bullet(rainbowBullet(i),bullet_radius,x,y+radius,bullet_v*cos,bullet_v*sin);
                        new_bullet->rotateAround(x,y+radius,0.012+0.0034*j,mode);
                        connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                        new_bullet->fadein(1200);
                        new_bullets->push_back(new_bullet);
                    }
                }
            }
        } else if(shoot_timer==2*shoot_cd) {
            shoot_timer = 0;
            mode=!mode;
            for(int i=-3;i<=3;++i) {
                phi = i*2*M_PI/7+(mode?M_PI/7:0)+angle;
                new_laser = new Laser(rainbowLaser(i),8,phi,mode?M_PI*2/775:-M_PI*2/775,75,x,y+radius,100);
                connect(this,SIGNAL(killItsBullets()),new_laser,SLOT(killItself()));
                new_bullets->push_back(new_laser);
            }
            new_bullet = new Bullet(":/res/bullet/2/purple.png",22,x,y+radius);
            new_bullet->setInvulnerable();
            new_bullet->fadein(1000);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
            connect(new_laser,SIGNAL(deadSignal()),new_bullet,SLOT(killItself()));
        }
        return new_bullets;
    }
    return nullptr;
}
