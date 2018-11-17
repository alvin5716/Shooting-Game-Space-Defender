#include "enemy_blue_5.h"
#include "bullet_nether.h"
#include "laser.h"
#include <QDebug>

Enemy_Blue_5::Enemy_Blue_5(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_Blue(player,220,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    first_time=true;
}
void Enemy_Blue_5::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy4_2.png";
        shoot_timer = -400;
        shoot_cd = 360;
        skill_timer = -425;
        emit useSkill("扭曲虛空");
        moveTo(Game::FrameWidth/2,100);
    },
    [this](){
        //skill timer
        if(skill_timer<=0) ++skill_timer;
        if(skill_timer==0) invulnerable=false;
    });
}
std::vector<Bullet*>* Enemy_Blue_5::shoot2() {
    static int attack2_count = 3;
    static double phi;
    static bool clockwise;
    std::vector<Bullet*>* new_bullets = new std::vector<Bullet*>;
    Bullet* new_bullet;
    //purple bullets
    int bullet_count = 28;
    if(attack2_count==3 && (shoot_timer==0 || shoot_timer==160)) phi = (qrand()%5)*2*M_PI/bullet_count/5;
    if(attack2_count==3 && shoot_timer==0) clockwise=(qrand()%2==0);
    if(attack2_count==3 && shoot_timer==160) clockwise=!clockwise;
    if(attack2_count==3 && ((shoot_timer>=50&&shoot_timer<=150) || (shoot_timer>=200&&shoot_timer<=300)) && shoot_timer%10==0) {
        double cos, sin;
        for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
            cos = std::cos(2*i*M_PI/bullet_count+phi);
            sin = std::sin(2*i*M_PI/bullet_count+phi);
            new_bullet = new Bullet(":/res/bullet_purple.png",12,x+radius*cos,y+radius*sin,0.005,0.005);
            new_bullet->rotateAround(x,y,0.018,clockwise);
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
        double bullet_v, bullet_a, angle, cos, sin;
        int bullet_radius, bullet_count;
        //bullet v, a
        bullet_v = -0.4;
        bullet_a = -0.0001;
        bullet_count = 22;
        bullet_radius = 50;
        angle=angleofvector(player->getX()-x,player->getY()-y);
        //shoot
        for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
            if(first_time && i==0) {
                first_time=false;
                continue;
            }
            cos = std::cos(angle+2*i*M_PI/bullet_count+((attack2_count!=0)?M_PI/bullet_count:0));
            sin = std::sin(angle+2*i*M_PI/bullet_count+((attack2_count!=0)?M_PI/bullet_count:0));
            new_bullet = new Bullet_Nether(QString(":/res/bullet_black.png"),bullet_radius,this,2000,x+950*cos,y+950*sin,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        ++attack2_count;
        shoot_timer = 0;
        return new_bullets;
    }
    return nullptr;
}
