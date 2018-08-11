#include "enemy_3_blue_5.h"
#include "bullet_time.h"
#include "bullet_wall.h"
#include "laser.h"
#include <QDebug>
#include "game.h"

Enemy_3_Blue_5::Enemy_3_Blue_5(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_3_Blue(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    shoot_times=1;
    interval=150;
}
void Enemy_3_Blue_5::skill() {
    //second phase
    if(health<=500 && !secPhase) {
        secPhase = true;
        invulnerable=true;
        attackable=false;
        img=":/res/enemy15_2.png";
        shoot_timer = -450;
        shoot_cd = 600;
        skill_timer = 0;
        emit useSkill("鳳凰會的密令");
        fadeout(3000);
    }
    if(secPhase) {
        //skill timer
        if(++skill_timer>25) {
            skill_timer=0;
            --this->health;
            emit healthChanged(health);
            if(health<=0) {
                dead=true;
                emit deadSignal();
            }
        }
    } else {
        Enemy_3_Blue::skill();
    }
}
std::vector<Bullet*>* Enemy_3_Blue_5::shoot2() {
    std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
    Bullet* new_bullet;
    Effect* new_effect;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        new_effect = new Effect(":/res/magic_red.png",70,70,230,230,350,player->getX(),player->getY(),0,0,0,0,true);
        new_effect->setOpacity(0.6);
        new_effect->setZValue(-1);
        new_effect->fadein();
        new_effect->rotateStart();
        emit summonEffect(new_effect);
        /*
        double cos, sin, bullet_v, bullet_a;
        int t = (shoot_timer-shoot_cd)/interval, bullet_radius, bullet_count = 24;
        bullet_radius = 14;
        bullet_v = 0.6;
        bullet_a = 0.005;
        angle = 0;
        for(int i=-(bullet_count/2);i<=(bullet_count/2-((bullet_count%2==0)?1:0));++i) {
            sin = std::sin(angle+(t*0.5+i)*2*M_PI/bullet_count);
            cos = std::cos(angle+(t*0.5+i)*2*M_PI/bullet_count);
            new_bullet = new Bullet(":/res/bullet_yellow.png",bullet_radius,x,y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        */
        if(shoot_timer==shoot_cd+interval*(shoot_times-1)) shoot_timer=0;
    }
    if(new_bullets->size()>0) return new_bullets;
    return NULL;
}
