#include "enemy_blue_3.h"
#include "laser.h"
#include <ctime>
#include <QDebug>

Enemy_Blue_3::Enemy_Blue_3(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_Blue(player,200,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    shootingbullet=false;
}
void Enemy_Blue_3::skill() {
    //second phase
    testIfSecPhase([this](){
        phi=0;
        usinglaser = true;
        count=0;
        bullet_count=24;
        invulnerable=true;
        img=":/res/enemy/1/blue_2.png";
        shoot_timer = -100;
        shoot_cd = 45;
        skill_timer = -200;
        emit useSkill("星爆氣流");
    },
    [this](){
        //skill
        if(skill_timer>=0) {
            if(skill_timer>=250*3) skill_timer=0;
            if(skill_timer==250*0+175) {
                moveTo(180,120);
                shoot_timer = 20;
                shoot_cd = 35;
                bullet_count = 32;
                bullet_v_t = 4;
                invulnerable=false;
            } else if(skill_timer==250*1) {
                shootingbullet = true;
                moveTo(Game::FrameWidth-180,120,188);
            } else if(skill_timer==250*2) {
                moveTo(Game::FrameWidth/2,300,188);
            } else if(skill_timer==250*2+188) {
                usinglaser = true;
                count=0;
            }
        } else if(skill_timer==-110) moveTo(Game::FrameWidth/2,150);
        //skill timer
        ++skill_timer;
    });
}
std::vector<Bullet*>* Enemy_Blue_3::shoot2() {
    if(shoot_timer>=shoot_cd) {
        std::vector<Bullet*>* new_bullets = new std::vector<Bullet*>;
        Bullet* new_bullet, *new_laser;
        //shoot
        double bullet_v=11, bullet_a=-0.6, sin, cos;
        if(shootingbullet) {
            for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
                sin=std::sin(2*i*M_PI/bullet_count);
                cos=std::cos(2*i*M_PI/bullet_count);
                new_bullet = new Bullet(":/res/bullet/1/purple.png",8,x,y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                new_bullet->setVTerminal(bullet_v_t);
                new_bullet->fadein(800);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
        if(usinglaser && (count==0 || count==3)){
            //laser
            QString str=(count==0)?":/res/bullet/laser/blue.png":":/res/bullet/laser/black.png";
            new_laser = new Laser(str,30,(count==0)?-5*M_PI/8-M_PI/5:5*M_PI/8+M_PI/5,(count==0)?M_PI/275:-M_PI/275,188,x,y,50);
            connect(this,SIGNAL(killItsBullets()),new_laser,SLOT(killItself()));
            new_bullets->push_back(new_laser);
            //bullet
            QString str2=(count==0)?":/res/bullet/1/blue.png":":/res/bullet/1/black.png";
            new_bullet = new Bullet(str2,40,x,y);
            new_bullet->setInvulnerable();
            new_bullet->fadein(1200);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            connect(new_laser,SIGNAL(deadSignal()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
            if(count==3) usinglaser=false;
        }
        ++count;
        shoot_timer = 0;
        return new_bullets;
    }
    return nullptr;
}
