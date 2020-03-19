#include "enemy_blue_5.h"
#include "bullet_nether.h"
#include "laser.h"
#include <QDebug>

Enemy_Blue_5::Enemy_Blue_5(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_Blue(player,220,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    first_time=true;
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
}
void Enemy_Blue_5::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        setImg(ImgPixmap::Level1::enemy_1_blue_2);
        shoot_timer = -200;
        shoot_cd = 180;
        skill_timer = -213;
        emit useSkill("扭曲虛空");
    },
    [this](){
        //skill timer
        if(skill_timer<=0) ++skill_timer;
        if(skill_timer==-100) moveTo(Game::FrameWidth/2,100);
        else if(skill_timer==0) invulnerable=false;
    });
}
std::vector<Bullet*>* Enemy_Blue_5::shoot2() {
    static int attack2_count = 3;
    static double phi;
    static bool clockwise;
    std::vector<Bullet*>* new_bullets = new std::vector<Bullet*>;
    Bullet* new_bullet;
    //purple bullets
    int bullet_count = 24;
    if(attack2_count==3 && (shoot_timer==0 || shoot_timer==80)) phi = (qrand()%5)*2*M_PI/bullet_count/5;
    if(attack2_count==3 && shoot_timer==0) clockwise=(qrand()%2==0);
    if(attack2_count==3 && shoot_timer==80) clockwise=!clockwise;
    if(attack2_count==3 && ((shoot_timer>=25&&shoot_timer<=75) || (shoot_timer>=100&&shoot_timer<=150)) && shoot_timer%5==0) {
        double cos, sin;
        for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
            cos = std::cos(2*i*M_PI/bullet_count+phi);
            sin = std::sin(2*i*M_PI/bullet_count+phi);
            new_bullet = new Bullet(ImgPixmap::Level1::bullet_1_purple,12,x+radius*cos,y+radius*sin,0.01,0.01);
            new_bullet->rotateAround(x,y,0.072,clockwise);
            new_bullet->fadein(2000);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        if(shoot_timer==150) {
            attack2_count=0;
        }
        return new_bullets;
    }
    //black bullets
    if(shoot_timer>=shoot_cd) {
        double bullet_v, bullet_a, angle, cos, sin;
        int bullet_radius, bullet_count, spawnpoint_radius;
        //bullet v, a
        spawnpoint_radius = 950;
        bullet_v = 0.8;
        bullet_a = 0.0004;
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
            new_bullet = new Bullet(ImgPixmap::Level1::bullet_1_black,bullet_radius,x+spawnpoint_radius*cos,y+spawnpoint_radius*sin,bullet_v*-cos,bullet_v*-sin,bullet_a*-cos,bullet_a*-sin);
            new_bullet->setCanOutOfFrame(true,1500);
            new_bullet->addTimeData(830,31,true);
            new_bullet->setOpacity(0.7);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        ++attack2_count;
        shoot_timer = 0;
        return new_bullets;
    }
    return nullptr;
}
