#include "enemy_3_blue_4.h"
#include "bullet_time.h"
#include "bullet_wall.h"
#include "laser.h"
#include <QDebug>
#include "game.h"

Enemy_3_Blue_4::Enemy_3_Blue_4(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_3_Blue(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    angle=0;
}
void Enemy_3_Blue_4::skill() {
    //second phase
    if(health<=250 && !secPhase) {
        secPhase = true;
        invulnerable=true;
        img=":/res/enemy15_2.png";
        shoot_timer = -950;
        shoot_cd = 85;
        skill_timer = -200;
        emit useSkill("火盃的考驗");
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
    if(shoot_timer==-550) {
        int aim_x, aim_y;
        Bullet_Time* new_bullet_time;
        for(int j=0;j<2;++j) {
            for(int i=0;i<20;++i) {
                new_bullet = new_bullet_time = new Bullet_Time(QString(":/res/bullet_purple.png"),8,x,y);
                if(i<5) {
                    aim_x=this->x-40-10*(4-i);
                    aim_y=this->y-15*(2-i);
                } else if(i<8) {
                    aim_x=this->x-20*(6-i);
                    aim_y=this->y+30;
                } else if(i<13) {
                    aim_x=this->x+40+10*(i-8);
                    aim_y=this->y+15*(10-i);
                } else {
                    aim_x=this->x+20*(16-i);
                    aim_y=this->y-30;
                }
                new_bullet->moveTo(aim_x,aim_y,50);
                new_bullet->fadein();
                new_bullet->setInvulnerable();
                new_bullet_time->addTimeData(50)
                        .addTimeData(100,aim_x+(j==0?-1:1)*(Game::FrameWidth/2-80),aim_y-50,250)
                        .addTimeData(250);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
    //laser
    } else if(shoot_timer==-150) {
        for(int i=0;i<2;++i) {
            new_bullet = new Laser(QString(":/res/laser_purple.png"),24,0,0,-1,i==0?80:Game::FrameWidth-80,230);
            new_bullet->setInvulnerable();
            new_bullet->setZValue(-1);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
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
            bullet_v = 0.1+(double)(qrand()%10)/10;
            cos = std::cos(angle+i*M_PI/5);
            sin = std::sin(angle+i*M_PI/5);
            new_bullet = new_bullet_wall = new Bullet_Wall(QString(":/res/bullet_red.png"),8,t==0?80:Game::FrameWidth-80,170,bullet_v*cos,bullet_v*sin,0,bullet_a);
            new_bullet_wall->addWallData(player,1.8,0);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        //yellow bullets
        if(this->health<130 && t==0) {
            int bullet_count = 26;
            bullet_v = 0.6;
            bullet_radius = 14;
            bullet_a = 0.0005;
            for(int i=-(bullet_count/2);i<=(bullet_count/2-((bullet_count%2==0)?1:0));++i) {
                sin = std::sin(angle+(t*0.5+i)*2*M_PI/bullet_count);
                cos = std::cos(angle+(t*0.5+i)*2*M_PI/bullet_count);
                new_bullet = new Bullet(":/res/bullet_yellow.png",bullet_radius,x,y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
        angle += M_PI/20;
        if(shoot_timer==shoot_cd+interval) shoot_timer=0;
    }
    if(new_bullets->size()>0) return new_bullets;
    return NULL;
}