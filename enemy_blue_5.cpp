#include "enemy_blue_5.h"
#include "bullet_nether.h"
#include "bullet_rotate.h"
#include "laser.h"
#include <QDebug>

Enemy_Blue_5::Enemy_Blue_5(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_Blue(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{

}
void Enemy_Blue_5::skill() {
    //second phase
    if(health<=250 && !secPhase) {
        secPhase = true;
        img=":/res/enemy4_2.png";
        shoot_timer = -500;
        shoot_cd = 360;
        skill_timer = -425;
        emit useSkill("扭曲虛空");
        moveTo(Game::FrameWidth/2,100);
    }
    if(secPhase) {
        //skill timer
        if(skill_timer<=0) ++skill_timer;
    }
}
std::vector<Bullet*>* Enemy_Blue_5::shoot2() {
    static int attack2_count = 3;
    static double phi;
    static bool clockwise;
    std::vector<Bullet*>* new_bullets = new std::vector<Bullet*>;
    Bullet* new_bullet, *new_laser;
    //laser
    if(shoot_timer==-450) {
        new_laser = new Laser(":/res/laser_purple.png",20,-M_PI/2,0,700,0,21);
        connect(this,SIGNAL(killItsBullets()),new_laser,SLOT(killItself()));
        new_bullets->push_back(new_laser);
        return new_bullets;
    } else if(shoot_timer==-325) {
        new_laser = new Laser(":/res/laser_purple.png",20,0,0,700,21,0);
        connect(this,SIGNAL(killItsBullets()),new_laser,SLOT(killItself()));
        new_bullets->push_back(new_laser);
        new_laser = new Laser(":/res/laser_purple.png",20,0,0,700,Game::FrameWidth-21,0);
        connect(this,SIGNAL(killItsBullets()),new_laser,SLOT(killItself()));
        new_bullets->push_back(new_laser);
        return new_bullets;
    } else if(shoot_timer==-200) {
        new_laser = new Laser(":/res/laser_purple.png",20,-M_PI/2,0,700,0,Game::FrameHeight-21);
        connect(this,SIGNAL(killItsBullets()),new_laser,SLOT(killItself()));
        new_bullets->push_back(new_laser);
        return new_bullets;
    }
    //purple bullets
    int bullet_count = 28;
    if(attack2_count==3 && (shoot_timer==0 || shoot_timer==160)) phi = (qrand()%5)*2*M_PI/bullet_count/5;
    if(attack2_count==3 && shoot_timer==0) clockwise=(qrand()%2==0)?true:false;
    if(attack2_count==3 && shoot_timer==160) clockwise=!clockwise;
    if(attack2_count==3 && ((shoot_timer>=50&&shoot_timer<=150) || (shoot_timer>=200&&shoot_timer<=300)) && shoot_timer%10==0) {
        static double cosa, sina;
        double cos, sin;
        if(shoot_timer==50) sincostoxy(sina,cosa,player->getX(),player->getY());
        for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
            cos = std::cos(2*i*M_PI/bullet_count+phi);
            sin = std::sin(2*i*M_PI/bullet_count+phi);
            new_bullet = new Bullet_Rotate(":/res/bullet_purple.png",x,y,0.018,clockwise,12,x+radius*cos,y+radius*sin,0.005,0.005);
            new_bullet->fadein(2000);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        if(shoot_timer==300) {
            attack2_count=0;
        }
        return new_bullets;
    }
    //black bullets
    if(shoot_timer>=shoot_cd) {
        double bullet_v, bullet_a, cosa, sina, cos, sin;
        int bullet_radius, bullet_count;
        //bullet v, a
        bullet_v = -0.4;
        bullet_a = -0.0001;
        bullet_count = 22;
        bullet_radius = 50;
        sincostoxy(sina,cosa,player->getX(),player->getY());
        //shoot
        for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
            cos = std::cos(2*i*M_PI/bullet_count+((attack2_count==0)?M_PI/bullet_count:0))*cosa-std::sin(2*i*M_PI/bullet_count+((attack2_count==0)?M_PI/bullet_count:0))*sina;
            sin = std::cos(2*i*M_PI/bullet_count+((attack2_count==0)?M_PI/bullet_count:0))*sina+std::sin(2*i*M_PI/bullet_count+((attack2_count==0)?M_PI/bullet_count:0))*cosa;
            new_bullet = new Bullet_Nether(QString(":/res/bullet_black.png"),bullet_radius,this,2000,x+950*cos,y+950*sin,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        ++attack2_count;
        shoot_timer = 0;
        return new_bullets;
    }
    return NULL;
}
