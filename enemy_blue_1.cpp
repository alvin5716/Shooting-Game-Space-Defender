#include "enemy_blue_1.h"
#include <QDebug>
#include "bullet_bounce.h"

Enemy_Blue_1::Enemy_Blue_1(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_Blue(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    clockwise=false;
    setDisappearTime(2000);
}
void Enemy_Blue_1::skill() {
    //second phase
    if(health<=250 && !secPhase) {
        secPhase = true;
        img=":/res/enemy4_2.png";
        shoot_timer = -225;
        shoot_cd = 250;
        skill_timer = -250;
        emit useSkill("殞落星斗");
    }
    if(secPhase) {
        //skill
        if(skill_timer>=0&&skill_timer%520==0) {
            if(skill_timer>=520*3) skill_timer=0;
            if(skill_timer==520*0) {
                moveTo(250,200,300);
            } else if(skill_timer==520*1) {
                moveTo(Game::FrameWidth-250,200,300);
            } else if(skill_timer==520*2) {
                moveTo(Game::FrameWidth/2,400,300);
            }
        }
        //skill timer
        ++skill_timer;
    }
}
std::vector<Bullet*>* Enemy_Blue_1::shoot2() {
    static double cosa, sina;
    const int total_t = 3;
    const int interval = 5;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        double bullet_v, bullet_a, cosb, sinb, cos, sin, cosp, sinp;
        int bullet_radius, bullet_count, t;
        std::vector<Bullet*>* new_bullets = new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a
        t = (shoot_timer-shoot_cd)/interval;
        bullet_v = 0.5;
        bullet_a = 0.000015;
        bullet_count = 15;
        bullet_radius = 8;
        if(shoot_timer==shoot_cd) {
            sincostoxy(sina,cosa,player->getX(),player->getY());
            clockwise=!clockwise;
        }
        //shoot
        for(int i=-(bullet_count/2);i<=(bullet_count/2-((bullet_count%2==0)?1:0));++i) {
            cosb = std::cos(2*i*M_PI/bullet_count);
            sinb = std::sin(2*i*M_PI/bullet_count);
            cos = cosa*cosb-sina*sinb;
            sin = sina*cosb+cosa*sinb;
            cosp = std::cos(5*M_PI/3);
            sinp = std::sin(((clockwise)?1:-1)*5*M_PI/3);
            new_bullet = new Bullet_Bounce(QString(":/res/bullet_purple.png"),3,bullet_radius,x+(radius-40)*(cos*cosp-sin*sinp)+25*cos*t,y+(radius-40)*(sin*cosp+cos*sinp)+25*sin*t,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            new_bullet->fadein(1500);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        if(shoot_timer==shoot_cd+interval*(total_t-1)) shoot_timer = 0;
        return new_bullets;
    }
    return NULL;
}
Effect* Enemy_Blue_1::disappear() const {
    Effect* corpse = new Effect(img,img_w,img_h,show_w,show_h,disappearTime/8,x,y);
    corpse->fadeout(disappearTime);
    return corpse;
}
