#include "enemy_2_blue_1.h"
#include <QDebug>

Enemy_2_Blue_1::Enemy_2_Blue_1(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_2_Blue(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    mode=false;
}
void Enemy_2_Blue_1::skill() {
    //second phase
    if(health<=180 && !secPhase) {
        secPhase = true;
        invulnerable=true;
        img=":/res/enemy10_2.png";
        shoot_timer = -420;
        shoot_cd = 100;
        skill_timer = -200;
        emit useSkill("炫彩流");
    }
    if(secPhase) {
        //skill
        if(skill_timer==0) moveTo(Game::FrameWidth/2-180,200,240);
        //skill timer
        if(skill_timer<=0) ++skill_timer;
    }
}
std::vector<Bullet*>* Enemy_2_Blue_1::shoot2() {
    const int interval=12;
    if(shoot_timer==shoot_cd+200) {
        if(mode) moveTo(Game::FrameWidth/2-180,200,620);
        else moveTo(Game::FrameWidth/2+180,200,620);
    }
    if((shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0)||(shoot_timer==shoot_cd+interval*110+200)) {
        double bullet_v, bullet_a, cos, sin;
        int bullet_radius, t;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a
        t = (shoot_timer-shoot_cd)/interval;
        bullet_radius = 9;
        if(shoot_timer==shoot_cd) {
            invulnerable=false;
            angle = angleofvector(player->getX()-x,player->getY()-y);
        }
        //shoot
        if(shoot_timer<=shoot_cd+interval*110) {
            for(int i=-3;i<=3;++i) {
                bullet_v = mode?0.9:1.4;
                bullet_a = mode?(i+2)*0.00062:i*0.00055-0.0001;
                cos = std::cos(angle+i*M_PI/24+(mode?-1:1)*t*M_PI/17.7);
                sin = std::sin(angle+i*M_PI/24+(mode?-1:1)*t*M_PI/17.7);
                new_bullet = new Bullet(rainbowBullet(i),bullet_radius,x,y+radius,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                new_bullet->setVTerminal(0.6);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullet->fadein(1800);
                new_bullets->push_back(new_bullet);
            }
        } else if(shoot_timer==shoot_cd+interval*110+200) {
            shoot_timer = 0;
            mode=!mode;
        }
        return new_bullets;
    }
    return nullptr;
}
