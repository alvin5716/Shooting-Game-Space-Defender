#include "enemy_3_blue.h"
#include "bullet_sin.h"
#include "game.h"

Enemy_3_Blue::Enemy_3_Blue(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_3(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    point=15;
    mode=false;
    setInvulnerable();
    skill_timer=-200-shoot_cd_init;
    boss=true;
    connect(this,SIGNAL(useSkill(QString)),this,SIGNAL(killItsBullets()));
    connect(this,SIGNAL(deadSignal()),this,SIGNAL(killItsBullets()));
    connect(this,SIGNAL(deadSignal()),this,SLOT(deadSet()));
    fadein(250);
}
void Enemy_3_Blue::deadSet() {
    emit deadSignal(x,y);
    emit useSkill("");
}
void Enemy_3_Blue::skill() {
    const int interval = 700;
    if(skill_timer>=0&&skill_timer%interval==0) {
        if(skill_timer>=interval*2) skill_timer=0;
        if(skill_timer==0) {
            moveTo(Game::FrameWidth/2+100+qrand()%(Game::FrameWidth/2-300),180+qrand()%50,375);
        } else if(skill_timer==interval*1) {
            moveTo(Game::FrameWidth/2-100-qrand()%(Game::FrameWidth/2-300),180+qrand()%50,375);
        }
    }
    ++skill_timer;
}
std::vector<Bullet*>* Enemy_3_Blue::shoot() {
    const int interval=30;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        double bullet_v, cos, sin, shoot_angle;
        int bullet_count, t;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a and count
        t = (shoot_timer-shoot_cd)/interval;
        bullet_count = 30;
        if(shoot_timer==shoot_cd) {
            angle = angleofvector(player->getX()-x,player->getY()-y);
            setVulnerable();
        }
        //shoot
        for(int i=-6;i<=6;++i) {
            bullet_v = 1.4+std::abs(i)*0.06;
            if (angle>M_PI/5&&angle<4*M_PI/5) shoot_angle = angle+i*M_PI/(bullet_count/2)+M_PI+t*(mode?1:-1)*M_PI/(bullet_count*2.5);
            else shoot_angle = i*M_PI/(bullet_count/2)-M_PI/2+t*(mode?1:-1)*M_PI/(bullet_count*2.5);
            cos = std::cos(shoot_angle);
            sin = std::sin(shoot_angle);
            new_bullet = new Bullet_Sin(QString(":/res/bullet_blue.png"),250,20,8,x,y,bullet_v*cos,bullet_v*sin,0,0.008);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        if(shoot_timer==shoot_cd+interval*3) {
            mode=!mode;
            shoot_timer = 0;
        }
        return new_bullets;
    }
    return NULL;
}
