#include "enemy_2_blue_2.h"
#include <QDebug>
#include "laser.h"

Enemy_2_Blue_2::Enemy_2_Blue_2(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_2_Blue(player,240,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    aim_summoned=false;
    shoot_count=-20;
}
void Enemy_2_Blue_2::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy/2/blue_2.png";
        shoot_timer = -210;
        shoot_cd = 20;
        skill_timer = -100;
        emit useSkill("七色光雨");
    },
    [this](){
        //skill
        if(skill_timer==0) moveTo(Game::FrameWidth/2,325,120);
        //skill timer
        if(skill_timer<=0) ++skill_timer;
    });
}
std::vector<Bullet*>* Enemy_2_Blue_2::shoot2() {
    if(!aim_summoned) {
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        for(int i=0;i<7;++i) {
            aim[i] = new Bullet(rainbowBullet(i-3),7,((double)Game::FrameWidth*(i+1))/8,8,2+i*0.4,0);
            aim[i]->addWallData(true);
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
        double bullet_v, bullet_a, cos, sin, angle;
        int bullet_radius;
        //bullet v, a
        bullet_radius = 6;
        bullet_v = 5;
        bullet_a = 0;
        //shoot
        for(int i=-3;i<=3;++i) {
            angle = angleofvector(aim[i+3]->getX()-x,aim[i+3]->getY()-radius-y);
            sin = std::sin(angle);
            cos = std::cos(angle);
            new_bullet = new Bullet(rainbowBullet(i),bullet_radius,x,y+radius,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            new_bullet->addWallData(0.1*(qrand()%40),0.01);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullet->fadein(1800);
            new_bullets->push_back(new_bullet);
        }
        switch (++shoot_count) {
        case 14:
            for(int i=0;i<2;++i) {
                angle = angleofvector(i*Game::FrameWidth-x,((player->getY()>Game::FrameHeight-17)?Game::FrameHeight-17:player->getY())-radius-y);
                sin = std::sin(angle);
                cos = std::cos(angle);
                stick[i] = new Bullet(":/res/bullet/2/purple.png",25,x,y+radius,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                stick[i]->addWallData();
                stick[i]->setInvulnerable();
                stick[i]->fadein(500);
                connect(this,SIGNAL(killItsBullets()),stick[i],SLOT(killItself()));
                new_bullets->push_back(stick[i]);
            }
            break;
        case 20:
            new_laser = new Laser(QString(":/res/bullet/laser/purple.png"),this,16,0,0,100,0,stick[0]->getY(),100);
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
    return nullptr;
}
