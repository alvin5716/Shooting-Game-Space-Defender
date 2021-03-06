#include "enemy_3_blue_4.h"
#include "bullet_sin.h"
#include "laser.h"
#include <QDebug>
#include "game.h"

Enemy_3_Blue_4::Enemy_3_Blue_4(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_3_Blue(player,230,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    angle=0;
    shoot_timer_2=0;
}
void Enemy_3_Blue_4::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy/3/blue_2.png";
        shoot_timer = -400;
        shoot_cd = 80;
        skill_timer = -100;
        emit useSkill("火盃的考驗");
        this->redMagicShield();
    },
    [this](){
        //skill
        if(skill_timer==0) moveTo(Game::FrameWidth/2,250,100);
        //skill timer
        if(skill_timer<=0) ++skill_timer;
    },
    [this](){
        Enemy_3_Blue::skill();
    });
}
std::vector<Bullet*>* Enemy_3_Blue_4::shoot2() {
    std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
    Bullet* new_bullet;
    //bowl
    if(shoot_timer==-200) {
        int aim_x, aim_y;
        for(int j=0;j<2;++j) {
            for(int i=0;i<20;++i) {
                new_bullet = new Bullet(QString(":/res/bullet/3/purple.png"),8,shootXPos(),shootYPos());
                if(i<5) {
                    aim_x=shootXPos()-40-10*(4-i);
                    aim_y=shootYPos()-15*(2-i);
                } else if(i<8) {
                    aim_x=shootXPos()-20*(6-i);
                    aim_y=shootYPos()+30;
                } else if(i<13) {
                    aim_x=shootXPos()+40+10*(i-8);
                    aim_y=shootYPos()+15*(10-i);
                } else {
                    aim_x=shootXPos()+20*(16-i);
                    aim_y=shootYPos()-30;
                }
                new_bullet->moveTo(aim_x,aim_y,25);
                new_bullet->fadein();
                new_bullet->setInvulnerable();
                new_bullet->addTimeData(25)
                        ->addTimeData(50,aim_x+(x-shootXPos())+(j==0?-1:1)*(Game::FrameWidth/2-80),aim_y+(y-shootYPos())-50,125)
                        ->addTimeData(125);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
    //flame
    } else if(shoot_timer>=shoot_cd) {
        setVulnerable();
        double cos, sin, bullet_v, bullet_a;
        int bullet_radius;
        bullet_radius = 8;
        bullet_a = -0.02;
        for(int j=0;j<2;++j) {
            for(int i=0;i<10;++i) {
                bullet_v = 0.2+(double)(qrand()%7)/10;
                cos = std::cos(angle+i*M_PI/5);
                sin = std::sin(angle+i*M_PI/5);
                new_bullet = new Bullet(QString(":/res/bullet/1/red.png"),bullet_radius,j==0?80:Game::FrameWidth-80,170,bullet_v*cos,bullet_v*sin,0,bullet_a);
                new_bullet->addWallData(player,3.6,0);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                connect(new_bullet,&Bullet::triggered,[=](){ new_bullet->setImg(":/res/bullet/3/red.png"); });
                new_bullets->push_back(new_bullet);
            }
        }
        angle += M_PI/20;
        shoot_timer=0;
    }
    //yellow bullets
    if(this->health<130) {
        const int shoot_cd_2 = 30;
        if(shoot_timer_2>=shoot_cd_2) {
            double cos, sin, bullet_v, bullet_a;
            int bullet_radius;
            angle2=angleofvector(player->getX()-x,player->getY()-y);
            bullet_v = 2;
            bullet_radius = 8;
            bullet_a = 0.002;
            for(int i=0;i<4;++i) {
                sin = std::sin(angle2+(i-1.5)*2*M_PI/15);
                cos = std::cos(angle2+(i-1.5)*2*M_PI/15);
                new_bullet = new Bullet_Sin(":/res/bullet/3/yellow.png",150,10,bullet_radius,shootXPos(),shootYPos(),bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
            shoot_timer_2=0;
        }
        ++shoot_timer_2;
    }
    if(new_bullets->size()>0) return new_bullets;
    return nullptr;
}
