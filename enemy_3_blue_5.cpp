#include "enemy_3_blue_5.h"
#include "bullet_nether.h"
#include "laser.h"
#include <QDebug>
#include "game.h"
#include "bullet_sin.h"
#include "effect_shaking.h"

Enemy_3_Blue_5::Enemy_3_Blue_5(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_3_Blue(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    angle=0;
    for(int i=0;i<7;++i) fireballs[i]=NULL;
    setDisappearTime(5000);
    centerball=NULL;
    randSeed=0;
    count=4;
}
void Enemy_3_Blue_5::skill() {
    //second phase
    if(health<=400 && !secPhase) {
        secPhase = true;
        invulnerable=true;
        img=":/res/enemy15_2.png";
        shoot_timer = -350;
        shoot_cd = 160;
        skill_timer = -200;
        emit useSkill("鳳凰會的密令");
        this->redMagicShield();
    }
    if(secPhase) {
        //skill
        if(skill_timer==0) moveTo(Game::FrameWidth/2,250,200);
        //skill timer
        if(skill_timer<=0) ++skill_timer;
        else Enemy_3_Blue::skill();
    } else {
        Enemy_3_Blue::skill();
    }
}
std::vector<Bullet*>* Enemy_3_Blue_5::shoot2() {
    std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
    Bullet* new_bullet;
    const int interval = 23;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        setVulnerable();
        double cos, sin, bullet_v, bullet_a;
        int bullet_radius, wings;
        wings = (health<290)?7:((health<350)?5:3);
        if(shoot_timer==shoot_cd) {
            angle=angleofvector(player->getX()-shootXPos(),player->getY()-shootYPos());
            bullet_radius = 16;
            bullet_a = 0;
            if(wings>=7) {
                if(++count>=4) {
                    randSeed=qrand()%4;
                    count=0;
                }
            }
            //fireballs
            for(int i=0;i<wings;++i) {
                switch((randSeed+count)%4) {
                case 0:
                    bullet_v = 2.35+std::abs(i-wings/2)*0.15;
                    break;
                case 1:
                    bullet_v = 2.25+(i-wings/2)*(i-wings/2)*0.125;
                    break;
                case 2:
                    bullet_v = 3.3-std::abs(i-wings/2)*0.15;
                    break;
                case 3:
                    bullet_v = 2.85;
                    break;
                }
                if(i==wings/2) {
                    cos = std::cos(angle);
                    sin = std::sin(angle);
                    bullet_v = 2.7;
                } else {
                    cos = std::cos(angle+(i>wings/2?6.5:-6.5)*M_PI/10-(i-wings/2)*M_PI/10);
                    sin = std::sin(angle+(i>wings/2?6.5:-6.5)*M_PI/10-(i-wings/2)*M_PI/10);
                }
                new_bullet = fireballs[i] = new Bullet_Nether(QString(":/res/bullet_2_red.png"),bullet_radius,NULL,500,shootXPos(),shootYPos(),bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                new_bullet->setInvulnerable();
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
            //circle
            if(health<250) {
                int bullet_count = 32;
                bullet_radius = 12;
                bullet_a = 0;
                bullet_v = 2.5;
                for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
                    cos = std::cos(angle+2*(i+0.5)*M_PI/bullet_count);
                    sin = std::sin(angle+2*(i+0.5)*M_PI/bullet_count);
                    new_bullet = new Bullet(QString(":/res/bullet_red.png"),bullet_radius,shootXPos(),shootYPos(),bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                    connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                    new_bullets->push_back(new_bullet);
                }
            }
            //fireballs (center)
            bullet_radius = 40;
            bullet_a = 0.01;
            bullet_v = -1.5;
            cos = std::cos(angle);
            sin = std::sin(angle);
            new_bullet = centerball = new Bullet_Nether(QString(":/res/bullet_2_red.png"),bullet_radius,NULL,1000,shootXPos()-22*cos,shootYPos()-22*sin,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            new_bullet->setVTerminal(1.6);
            new_bullet->setZValue(1);
            new_bullet->setInvulnerable();
            new_bullet->fadein(500);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        } else {
            //small balls
            bullet_radius = 9;
            bullet_a = 0.01;
            bullet_v = -1.5;
            for(int i=0;i<wings;++i) {
                if(fireballs[i]!=NULL) {
                    cos = std::cos(angle);
                    sin = std::sin(angle);
                    new_bullet = new Bullet_Nether(QString(":/res/bullet_red.png"),bullet_radius,NULL,1000,fireballs[i]->getX(),fireballs[i]->getY(),bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                    new_bullet->setVTerminal(1.6);
                    new_bullet->fadein((i==wings/2)?100:500);
                    connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                    new_bullets->push_back(new_bullet);
                }
            }
            //tail
            for(int i=0;i<2;++i) {
                bullet_radius = 9;
                bullet_a = 0;
                bullet_v = 1.6;
                cos = std::cos(angle+M_PI+((i==0)?M_PI/12:-M_PI/12));
                sin = std::sin(angle+M_PI+((i==0)?M_PI/12:-M_PI/12));
                new_bullet = new Bullet_Sin(QString(":/res/bullet_red.png"),30,8,bullet_radius,centerball->getX(),centerball->getY(),bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin,i==0);
                new_bullet->fadein(500);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
        if(shoot_timer==shoot_cd+interval*15) {
            shoot_timer=0;
            if(health<50) shoot_cd=70;
            else if(health<200) shoot_cd=90;
        }
    }
    if(new_bullets->size()>0) return new_bullets;
    return NULL;
}
Effect* Enemy_3_Blue_5::disappear() const {
    Effect* corpse = new Effect_Shaking(":/res/enemy15_3.png",img_w,img_h,show_w,show_h,disappearTime/8,x,y);
    corpse->fadeout(disappearTime);
    return corpse;
}
