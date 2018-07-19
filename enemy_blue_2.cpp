#include "enemy_blue_2.h"
#include "bullet_rotate.h"
#include <QDebug>

Enemy_Blue_2::Enemy_Blue_2(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_Blue(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    clockwise=false;
}
void Enemy_Blue_2::skill() {
    //second phase
    if(health<=140 && !secPhase) {
        secPhase = true;
        img=":/res/enemy4_2.png";
        shoot_timer = -175;
        shoot_cd = 250;
        skill_timer = -100;
        emit useSkill("螺旋星雲");
    }
    if(secPhase) {
        //skill
        if(skill_timer==0) moveTo(Game::FrameWidth/2,350);
        //skill timer
        if(skill_timer<=0) ++skill_timer;
    }
}
std::vector<Bullet*>* Enemy_Blue_2::shoot2() {
    static double cosa, sina;
    const int total_t = 7;
    const int interval = 125;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        double bullet_v, cosb, sinb, cos, sin;
        int bullet_count, t;
        std::vector<Bullet*>* new_bullets = new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a
        t = (shoot_timer-shoot_cd)/interval;
        bullet_v = 0.4;
        bullet_count = 24;
        if(shoot_timer==shoot_cd) {
            sincostoxy(sina,cosa,player->getX(),player->getY());
            clockwise=!clockwise;
        }
        //shoot
        //black balls
        if(t==2) {
            for(int i=0;i<2;++i) {
                new_bullet = new Bullet(QString(":/res/bullet_black.png"),35,(i==0)?220:Game::FrameWidth-220,-34,0,2.2,0,-0.01);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
        //rotate bullets
        if(t<4) {
            for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
                int bullet_radius=((i+(bullet_count/2))%4)*5+12;
                cosb = std::cos(2*i*M_PI/bullet_count+(M_PI*8*t/bullet_count/3));
                sinb = std::sin(2*i*M_PI/bullet_count+(M_PI*8*t/bullet_count/3));
                cos = cosa*cosb-sina*sinb;
                sin = sina*cosb+cosa*sinb;
                new_bullet = new Bullet_Rotate(QString(":/res/bullet_purple.png"),x,y,0.011,clockwise,bullet_radius,x+(double)radius*2/3*cos,y+(double)radius*2/3*sin,bullet_v*cos,bullet_v*sin);
                new_bullet->fadein(500);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        } else if(t==4) {
            for(int j=0;j<2;++j) {
                for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
                    cosb = std::cos(2*i*M_PI/bullet_count+M_PI/bullet_count);
                    sinb = std::sin(2*i*M_PI/bullet_count+M_PI/bullet_count);
                    cos = cosa*cosb-sina*sinb;
                    sin = sina*cosb+cosa*sinb;
                    new_bullet = new Bullet_Rotate(QString(":/res/bullet_purple.png"),(j==0)?220:Game::FrameWidth-220,200,0.01,clockwise,12,(j==0)?220:Game::FrameWidth-220,200,bullet_v*cos,bullet_v*sin);
                    connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                    new_bullets->push_back(new_bullet);
                }
            }
        }
        if(shoot_timer==shoot_cd+interval*(total_t-1)) shoot_timer = 0;
        return new_bullets;
    }
    return NULL;
}
