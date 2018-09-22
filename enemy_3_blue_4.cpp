#include "enemy_3_blue_4.h"
#include "bullet_time.h"
#include "bullet_wall.h"
#include "bullet_sin.h"
#include "laser.h"
#include <QDebug>
#include "game.h"

Enemy_3_Blue_4::Enemy_3_Blue_4(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_3_Blue(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    angle=0;
    shoot_timer_2=0;
}
void Enemy_3_Blue_4::skill() {
    //second phase
    if(health<=230 && !secPhase) {
        secPhase = true;
        invulnerable=true;
        img=":/res/enemy15_2.png";
        shoot_timer = -800;
        shoot_cd = 85;
        skill_timer = -200;
        emit useSkill("火盃的考驗");
        this->redMagicShield();
    }
    if(secPhase) {
        //skill
        if(skill_timer==0) moveTo(Game::FrameWidth/2,250,200);
        //skill timer
        if(skill_timer<=0) ++skill_timer;
    } else {
        Enemy_3_Blue::skill();
    }
}
std::vector<Bullet*>* Enemy_3_Blue_4::shoot2() {
    std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
    Bullet* new_bullet;
    const int interval = shoot_cd;
    //bowl
    if(shoot_timer==-400) {
        int aim_x, aim_y;
        Bullet_Time* new_bullet_time;
        for(int j=0;j<2;++j) {
            for(int i=0;i<20;++i) {
                new_bullet = new_bullet_time = new Bullet_Time(QString(":/res/bullet_purple.png"),8,shootXPos(),shootYPos());
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
                new_bullet->moveTo(aim_x,aim_y,50);
                new_bullet->fadein();
                new_bullet->setInvulnerable();
                new_bullet_time->addTimeData(50)
                        .addTimeData(100,aim_x+(x-shootXPos())+(j==0?-1:1)*(Game::FrameWidth/2-80),aim_y+(y-shootYPos())-50,250)
                        .addTimeData(250);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
    //flame
    } else if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        setVulnerable();
        Bullet_Wall* new_bullet_wall;
        double cos, sin, bullet_v, bullet_a;
        int t = (shoot_timer-shoot_cd)/interval, bullet_radius;
        bullet_radius = 8;
        bullet_a = -0.005;
        for(int i=0;i<10;++i) {
            bullet_v = 0.1+(double)(qrand()%4)/10;
            cos = std::cos(angle+i*M_PI/5);
            sin = std::sin(angle+i*M_PI/5);
            new_bullet = new_bullet_wall = new Bullet_Wall(QString(":/res/bullet_red.png"),bullet_radius,t==0?80:Game::FrameWidth-80,170,bullet_v*cos,bullet_v*sin,0,bullet_a);
            new_bullet_wall->addWallData(player,1.8,0);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            connect(new_bullet_wall,&Bullet_Wall::triggered,[=](){ new_bullet_wall->setImg(":/res/bullet_2_red.png"); });
            new_bullets->push_back(new_bullet);
        }
        angle += M_PI/20;
        if(shoot_timer==shoot_cd+interval) shoot_timer=0;
    }
    //yellow bullets
    if(this->health<130) {
        const int shoot_cd_2 = 55;
        if(shoot_timer_2>=shoot_cd_2) {
            double cos, sin, bullet_v, bullet_a;
            int bullet_radius;
            angle2=angleofvector(player->getX()-x,player->getY()-y);
            bullet_v = 1;
            bullet_radius = 8;
            bullet_a = 0.0005;
            for(int i=0;i<4;++i) {
                sin = std::sin(angle2+(i-1.5)*2*M_PI/15);
                cos = std::cos(angle2+(i-1.5)*2*M_PI/15);
                new_bullet = new Bullet_Sin(":/res/bullet_yellow.png",300,10,bullet_radius,shootXPos(),shootYPos(),bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
            shoot_timer_2=0;
        }
        ++shoot_timer_2;
    }
    if(new_bullets->size()>0) return new_bullets;
    return NULL;
}
