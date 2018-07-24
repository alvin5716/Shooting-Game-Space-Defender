#include "enemy_2_blue_2.h"
#include <QDebug>
#include "bullet_bounce.h"
#include "laser.h"
#include "bullet_wall_vertical.h"
#include "bullet_wall_stick.h"

Enemy_2_Blue_2::Enemy_2_Blue_2(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_2_Blue(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    aim_summoned=false;
    shoot_count=-20;
}
void Enemy_2_Blue_2::skill() {
    //second phase
    if(health<=240 && !secPhase) {
        secPhase = true;
        invulnerable=true;
        img=":/res/enemy10_2.png";
        shoot_timer = -420;
        shoot_cd = 40;
        skill_timer = -200;
        emit useSkill("七色光雨");
    }
    if(secPhase) {
        //skill
        if(skill_timer==0) moveTo(Game::FrameWidth/2,325,240);
        //skill timer
        if(skill_timer<=0) ++skill_timer;
    }
}
std::vector<Bullet*>* Enemy_2_Blue_2::shoot2() {
    if(!aim_summoned) {
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        for(int i=0;i<7;++i) {
            aim[i] = new Bullet_Bounce(rainbowBullet(i-3),-1,7,((double)Game::FrameWidth*(i+1))/8,8,1+i*0.2,0,0,0);
            aim[i]->setInvulnerable();
            aim[i]->setOpacity(0);
            connect(this,SIGNAL(killItsBullets()),aim[i],SLOT(killItself()));
            new_bullets->push_back(aim[i]);
        }
        aim_summoned=true;
        return new_bullets;
    }
    if(shoot_timer==shoot_cd) {
        invulnerable=false;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet, *new_laser;
        double bullet_v, bullet_a, cos, sin;
        int bullet_radius;
        //bullet v, a
        bullet_radius = 6;
        bullet_v = 2.5;
        bullet_a = 0;
        //shoot
        for(int i=-3;i<=3;++i) {
            sincostoxy(sin,cos,aim[i+3]->getX(),aim[i+3]->getY()-radius);
            new_bullet = new Bullet_Wall_Vertical(rainbowBullet(i),bullet_radius,1,0.05*(qrand()%40),0.0025,x,y+radius,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullet->fadein(1800);
            new_bullets->push_back(new_bullet);
        }
        switch (++shoot_count) {
        case 14:
            for(int i=0;i<2;++i) {
                sincostoxy(sin,cos,i*Game::FrameWidth,((player->getY()>Game::FrameHeight-17)?Game::FrameHeight-17:player->getY())-radius);
                stick[i] = new Bullet_Wall_Stick(":/res/bullet_2_purple.png",25,x,y+radius,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                stick[i]->setInvulnerable();
                stick[i]->fadein(500);
                connect(this,SIGNAL(killItsBullets()),stick[i],SLOT(killItself()));
                new_bullets->push_back(stick[i]);
            }
            break;
        case 20:
            new_laser = new Laser(QString(":/res/laser_purple.png"),16,-M_PI/2,0,150,0,stick[0]->getY(),200);
            connect(this,SIGNAL(killItsBullets()),new_laser,SLOT(killItself()));
            for(int i=0;i<2;++i) connect(new_laser,SIGNAL(deadSignal()),stick[i],SLOT(killItself()));
            new_bullets->push_back(new_laser);
            new_laser->setZValue(-1);
            shoot_count=0;
            break;
        }
        shoot_timer = 0;
        return new_bullets;
    }
    return NULL;
}
