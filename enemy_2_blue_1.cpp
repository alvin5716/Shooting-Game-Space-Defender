#include "enemy_2_blue_1.h"
#include <QDebug>

Enemy_2_Blue_1::Enemy_2_Blue_1(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_2_Blue(player,180,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    mode=false;
}
void Enemy_2_Blue_1::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy/2/blue_2.png";
        shoot_timer = -210;
        shoot_cd = 50;
        skill_timer = -100;
        emit useSkill("炫彩流");
    },
    [this](){
        //skill
        if(skill_timer==0) moveTo(Game::FrameWidth/2-180,200,120);
        //skill timer
        if(skill_timer<=0) ++skill_timer;
    },
    [this](){
        if(skill_timer==165) emit dialogueStart();
        if(skill_timer<=165) ++skill_timer;
    });
}
std::vector<Bullet*>* Enemy_2_Blue_1::shoot2() {
    const double coeff = 1.5;
    const int interval = 8;
    if(shoot_timer==shoot_cd+100) {
        if(mode) moveTo(Game::FrameWidth/2-180,200,360);
        else moveTo(Game::FrameWidth/2+180,200,360);
    }
    if((shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0)||(shoot_timer==shoot_cd+interval*55+40)) {
        double bullet_v, bullet_a, cos, sin;
        int bullet_radius, t;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a
        t = (shoot_timer-shoot_cd)/interval;
        bullet_radius = 7;
        if(shoot_timer==shoot_cd) {
            invulnerable=false;
            angle = angleofvector(player->getX()-x,player->getY()-y) + qrand()%30/30.0*(M_PI/15);
        }
        //shoot
        if(shoot_timer<=shoot_cd+interval*55) {
            for(int i=-3;i<=3;++i) {
                bullet_v = (mode?1.8:2.8)*coeff;
                bullet_a = (mode?(i+2)*0.00248:i*0.0022-0.0004)*coeff*coeff;
                double shoot_ang = angle+i*M_PI/24+(mode?-1:1)*t*M_PI/12.77;
                cos = std::cos(shoot_ang);
                sin = std::sin(shoot_ang);
                new_bullet = new Bullet(rainbowBullet(i),bullet_radius,x,y+radius,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                new_bullet->setVTerminal(1.8);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullet->fadein(1800);
                new_bullets->push_back(new_bullet);
            }
        } else if(shoot_timer==shoot_cd+interval*55+40) {
            shoot_timer = 0;
            mode=!mode;
        }
        return new_bullets;
    }
    return nullptr;
}
