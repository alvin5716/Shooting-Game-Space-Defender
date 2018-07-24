#include "enemy_2_blue_5.h"
#include "bullet_rotate.h"
#include "laser.h"
#include <QDebug>

Enemy_2_Blue_5::Enemy_2_Blue_5(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_2_Blue(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    mode=false;
}
void Enemy_2_Blue_5::skill() {
    //second phase
    if(health<=170 && !secPhase) {
        secPhase = true;
        invulnerable=true;
        img=":/res/enemy10_2.png";
        shoot_timer = -120;
        shoot_cd = 350;
        skill_timer = -200;
        emit useSkill("七彩霓虹燈");
    }
    if(secPhase) {
        //skill
        if(skill_timer==0) moveTo(Game::FrameWidth/2,300,240);
        //skill timer
        if(skill_timer<=0) ++skill_timer;
    }
}
std::vector<Bullet*>* Enemy_2_Blue_5::shoot2() {
    static double tana=0, cosa=0, sina=0;
    if(shoot_timer==shoot_cd || shoot_timer==2*shoot_cd) {
        double bullet_v, cosb, sinb, cos, sin, angle;
        int bullet_radius;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet, *new_laser;
        //bullet v, a
        bullet_radius = 8;
        if(shoot_timer==shoot_cd) {
            tana = (y-player->getY()) / (x-player->getX());
            if(std::isinf(tana)) cosa = 0;
            else cosa = ((player->getX()>x)?1:-1)/ sqrt(tana*tana+1);
            if(std::isinf(tana)) sina = 1;
            else sina = tana*cosa;
        }
        //shoot
        if(shoot_timer==shoot_cd) {
            invulnerable=false;
            for(int k=-1;k<=1;++k) {
                for(int j=0;j<7;++j) {
                    for(int i=-3;i<=3;++i) {
                        bullet_v = 0.15+0.25*j;
                        cosb = std::cos(i*2*M_PI/7+(mode?M_PI/7:0)+k*M_PI/14);
                        sinb = std::sin(i*2*M_PI/7+(mode?M_PI/7:0)+k*M_PI/14);
                        cos = cosa*cosb-sina*sinb;
                        sin = sina*cosb+cosa*sinb;
                        new_bullet = new Bullet_Rotate(rainbowBullet(i),x,y+radius,0.003+0.00085*j,mode,bullet_radius,x,y+radius,bullet_v*cos,bullet_v*sin);
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
                angle = i*2*M_PI/7+(mode?M_PI/7:0)-M_PI/2+((cosa>0)?std::asin(sina):M_PI-std::asin(sina));
                new_laser = new Laser(rainbowLaser(i),8,angle,mode?M_PI/775:-M_PI/775,150,x,y+radius,200);
                connect(this,SIGNAL(killItsBullets()),new_laser,SLOT(killItself()));
                new_bullets->push_back(new_laser);
            }
            new_bullet = new Bullet(":/res/bullet_2_purple.png",22,x,y+radius);
            new_bullet->setInvulnerable();
            new_bullet->fadein(1000);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
            connect(new_laser,SIGNAL(deadSignal()),new_bullet,SLOT(killItself()));
        }
        return new_bullets;
    }
    return NULL;
}
