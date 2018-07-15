#include "enemy_2_blue_1.h"
#include <QDebug>
#include "bullet_bounce.h"
#include "bullet_terminal.h"

Enemy_2_Blue_1::Enemy_2_Blue_1(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_2_Blue(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    mode=false;
}
void Enemy_2_Blue_1::skill() {
    //second phase
    if(health<=200 && !secPhase) {
        secPhase = true;
        img=":/res/enemy10_2.png";
        shoot_timer = -420;
        shoot_cd = 100;
        skill_timer = -200;
        emit useSkill("炫彩洪流");
    }
    if(secPhase) {
        //skill
        if(skill_timer==0) moveTo(Game::FrameWidth/2-180,200,240);
        //skill timer
        if(skill_timer<=0) ++skill_timer;
    }
}
std::vector<Bullet*>* Enemy_2_Blue_1::shoot2() {
    static double tana=0, cosa=0, sina=0;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%8==0) {
        double bullet_v, bullet_a, cosb, sinb, cos, sin;
        int bullet_radius, t;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a
        t = (shoot_timer-shoot_cd)/8;
        bullet_radius = 9;
        if(shoot_timer==shoot_cd) {
            tana = (y-player->getY()) / (x-player->getX());
            if(std::isinf(tana)) cosa = 0;
            else cosa = ((player->getX()>x)?1:-1)/ sqrt(tana*tana+1);
            if(std::isinf(tana)) sina = 1;
            else sina = tana*cosa;
        }
        //shoot
        if(shoot_timer<=shoot_cd+880) {
            if(shoot_timer==shoot_cd+200) {
                if(mode) moveTo(Game::FrameWidth/2-180,200,620);
                else moveTo(Game::FrameWidth/2+180,200,620);
            }
            for(int i=-3;i<=3;++i) {
                bullet_v = mode?0.9:1.4;
                bullet_a = mode?(i+2)*0.0006:i*0.0005;
                cosb = std::cos(i*M_PI/24+(mode?-1:1)*t*M_PI/16.7);
                sinb = std::sin(i*M_PI/24+(mode?-1:1)*t*M_PI/16.7);
                cos = cosa*cosb-sina*sinb;
                sin = sina*cosb+cosa*sinb;
                new_bullet = new Bullet_Terminal(rainbowBullet(i),bullet_radius,0.6,x,y+radius,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullet->fadein(1800);
                new_bullets->push_back(new_bullet);
            }
        } else if(shoot_timer==shoot_cd+1080) {
            shoot_timer = 0;
            mode=!mode;
        }
        return new_bullets;
    }
    return NULL;
}
