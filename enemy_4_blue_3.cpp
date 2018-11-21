#include "enemy_4_blue_3.h"
#include "enemy_temp.h"
#include "game.h"
#include <QDebug>

Enemy_4_Blue_3::Enemy_4_Blue_3(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_4_Blue(player,160,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    shoot_count=0;
    angle_seed=0;
    this->setBossHPToSkill();
}
void Enemy_4_Blue_3::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy20_2.png";
        shoot_timer = -270;
        shoot_cd = 50;
        skill_timer = -420;
        emit useSkill("膽小鬼賽局-以退為進");
    },
    [this](){
        //skill
        if(skill_timer==-220) moveTo(Game::FrameWidth/2,120,150);
        //skill timer
        if(skill_timer<0) {
            ++skill_timer;
        } else if(skill_timer==0) {
            ++skill_timer;
            this->bounceable = true;
            this->stopable = false;
            this->setAcceleration(0,0.004);
        } else {
            bool b = player->getX()>this->x;
            this->xa=b?0.008:-0.008;
            if(this->xv>1.2) this->xv=1.2;
            else if(this->xv<-1.2) this->xv=-1.2;
            if(this->y<125 && !this->waving) {
                this->waving=true;
                shoot_cd=25;
            }
        }
    },
    [this](){
        Enemy_4_Blue::skill();
    });
}
std::vector<Bullet*>* Enemy_4_Blue_3::shoot2() {
    if(shoot_timer>=shoot_cd) {
        setVulnerable();
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        double constexpr bullet_v=2.2;
        double sin, cos;
        for(int i=0;i<2;++i) {
            double angle = M_PI/2+(i?1:-1)*(M_PI/5-(std::min(280-angle_seed,angle_seed)/4)*M_PI/180);
            sin = std::sin(angle);
            cos = std::cos(angle);
            new_bullet = new Bullet(QString(":/res/bullet_purple.png"),14,x+cos*radius,y+sin*radius,bullet_v*cos,bullet_v*sin);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        shoot_timer=0;
        return new_bullets;
    }
    if( (waving && ++angle_seed>280) || !waving ) {
        angle_seed=0;
        waving = false;
        shoot_cd = 50;
    }
    return nullptr;
}
