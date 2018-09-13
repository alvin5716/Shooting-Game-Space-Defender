#include "enemy_blue_3.h"
#include "laser.h"
#include <ctime>
#include <QDebug>

Enemy_Blue_3::Enemy_Blue_3(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_Blue(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    shootingbullet=false;
}
void Enemy_Blue_3::skill() {
    //second phase
    if(health<=200 && !secPhase) {
        phi=0;
        usinglaser = true;
        count=0;
        bullet_count=24;
        secPhase = true;
        invulnerable=true;
        img=":/res/enemy4_2.png";
        shoot_timer = -200;
        shoot_cd = 90;
        skill_timer = -400;
        emit useSkill("星爆氣流");
        moveTo(Game::FrameWidth/2,100);
    }
    if(secPhase) {
        //skill
        if(skill_timer>=0) {
            if(skill_timer>=500*3) skill_timer=0;
            if(skill_timer==500*0+370) {
                moveTo(180,120);
                shoot_timer = 40;
                shoot_cd = 70;
                bullet_count = 32;
                bullet_v_t = 2;
                invulnerable=false;
            } else if(skill_timer==500*1) {
                shootingbullet = true;
                moveTo(Game::FrameWidth-180,120,375);
            } else if(skill_timer==500*2) {
                moveTo(Game::FrameWidth/2,300,375);
            } else if(skill_timer==500*2+375) {
                usinglaser = true;
                count=0;
            }
        }
        //skill timer
        ++skill_timer;
    }
}
std::vector<Bullet*>* Enemy_Blue_3::shoot2() {
    if(shoot_timer>=shoot_cd) {
        std::vector<Bullet*>* new_bullets = new std::vector<Bullet*>;
        Bullet* new_bullet, *new_laser;
        //shoot
        double bullet_v=5.5, bullet_a=-0.15, sin, cos;
        if(shootingbullet) {
            for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
                sin=std::sin(2*i*M_PI/bullet_count);
                cos=std::cos(2*i*M_PI/bullet_count);
                new_bullet = new Bullet(":/res/bullet_purple.png",8,x,y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                new_bullet->setVTerminal(bullet_v_t);
                new_bullet->fadein(800);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
        if(usinglaser && (count==0 || count==3)){
            //laser
            QString str=(count==0)?":/res/laser_blue.png":":/res/laser_black.png";
            new_laser = new Laser(str,30,(count==0)?-5*M_PI/8-M_PI/5:5*M_PI/8+M_PI/5,(count==0)?M_PI/550:-M_PI/550,370,x,y,100);
            connect(this,SIGNAL(killItsBullets()),new_laser,SLOT(killItself()));
            new_bullets->push_back(new_laser);
            //bullet
            QString str2=(count==0)?":/res/bullet_blue.png":":/res/bullet_black.png";
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
    return NULL;
}
