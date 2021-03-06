#include "enemy_blue_1.h"
#include <QDebug>

Enemy_Blue_1::Enemy_Blue_1(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_Blue(player,240,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    point+=10;
    clockwise=false;
    setDisappearTime(-1);
    bullet_fast=false;
    setDeathImg(":/res/enemy/1/blue_3.png");
}
void Enemy_Blue_1::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy/1/blue_2.png";
        shoot_timer = -113;
        shoot_cd = 125;
        skill_timer = -125;
        emit useSkill("殞落星斗");
    },
    [this](){
        //skill
        if(skill_timer>=0&&skill_timer%260==0) {
            if(skill_timer>=260*3) skill_timer=0;
            if(skill_timer==260*0) {
                moveTo(250,200,150);
            } else if(skill_timer==260*1) {
                moveTo(Game::FrameWidth-250,200,150);
            } else if(skill_timer==260*2) {
                moveTo(Game::FrameWidth/2,400,150);
            }
        }
        //skill timer
        ++skill_timer;
    });
}
std::vector<Bullet*>* Enemy_Blue_1::shoot2() {
    const int total_t = 3;
    const int interval = 3;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        double bullet_v, bullet_a, cos, sin, cosp, sinp;
        int bullet_radius, bullet_count, t;
        std::vector<Bullet*>* new_bullets = new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a
        t = (shoot_timer-shoot_cd)/interval;
        bullet_a = 0.00006;
        bullet_count = 20;
        bullet_radius = 8;
        if(shoot_timer==shoot_cd) {
            invulnerable=false;
            angle=angleofvector(player->getX()-x,player->getY()-y);
            clockwise=!clockwise;
        }
        //shoot
        for(int i=-(bullet_count/2);i<=(bullet_count/2-((bullet_count%2==0)?1:0));++i) {
            bullet_fast=!bullet_fast;
            bullet_v = (bullet_fast)?1.6:0.6;
            QString str = (bullet_fast)?":/res/bullet/1/purple.png":":/res/bullet/1/black.png";
            cos = std::cos(angle+2*i*M_PI/bullet_count);
            sin = std::sin(angle+2*i*M_PI/bullet_count);
            cosp = std::cos(5*M_PI/3);
            sinp = std::sin(((clockwise)?1:-1)*5*M_PI/3);
            new_bullet =  new Bullet(str,bullet_radius,x+(radius-40)*(cos*cosp-sin*sinp)+25*cos*t,y+(radius-40)*(sin*cosp+cos*sinp)+25*sin*t,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            for(int i=0;i<((bullet_fast)?3:1);++i) new_bullet->addWallData(false);
            new_bullet->fadein((bullet_fast)?1500:3000);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        if(shoot_timer==shoot_cd+interval*(total_t-1)) {
            shoot_timer = 0;
        }
        return new_bullets;
    }
    return nullptr;
}
