#include "enemy_3_blue_1.h"
#include "bullet_time.h"
#include "bullet_sin.h"
#include <QDebug>
#include "game.h"

Enemy_3_Blue_1::Enemy_3_Blue_1(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_3_Blue(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    mode=false;
    magicstone=NULL;
    shoot_small_bullets=false;
}
void Enemy_3_Blue_1::skill() {
    //second phase
    if(health<=170 && !secPhase) {
        secPhase = true;
        invulnerable=true;
        img=":/res/enemy15_2.png";
        shoot_timer = -700;
        shoot_cd = 600;
        skill_timer = -200;
        emit useSkill("神秘的魔法石");
    }
    if(secPhase) {
        //skill
        if(skill_timer==0) moveTo(Game::FrameWidth/2,180,240);
        //skill timer
        if(skill_timer<=0) ++skill_timer;
    } else {
        Enemy_3_Blue::skill();
    }
}
std::vector<Bullet*>* Enemy_3_Blue_1::shoot2() {
    //magic stone
    if(shoot_timer==-300) {
        double bullet_v, bullet_a, cos, sin;
        int bullet_radius;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        bullet_radius = 60;
        bullet_v = 0;
        bullet_a = 0.015;
        cos = std::cos(-M_PI/6);
        sin = std::sin(-M_PI/6);
        new_bullet = magicstone = new Bullet_Wall(":/res/magicball.png",bullet_radius,x,y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
        magicstone->addWallData(player,2.6);
        connect(magicstone,SIGNAL(triggered()),this,SLOT(shootSmallBullet()));
        new_bullet->setInvulnerable();
        connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
        new_bullets->push_back(new_bullet);
        return new_bullets;
    }
    std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
    Bullet* new_bullet;
    //small magic stone bullets
    if(shoot_small_bullets) {
        Bullet_Time* new_bullet_time;
        double bullet_v, bullet_a, cos, sin;
        int bullet_count;
        //bullet v, a and count
        bullet_v = 0.1;
        bullet_a = 0.008;
        bullet_count = 20;
        angle = angleofvector(player->getX()-x,player->getY()-y);
        //shoot
        for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
            double shoot_angle = angle+(i+(mode?0.5:0))*M_PI/(bullet_count/2);
            cos = std::cos(shoot_angle);
            sin = std::sin(shoot_angle);
            new_bullet = new_bullet_time =
                    new Bullet_Time(QString(":/res/magicball.png"),14,
                                    magicstone->getX()+magicstone->getRadius()*cos*0.5,
                                    magicstone->getY()+magicstone->getRadius()*sin*0.5,
                                    bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            for(int i=0;i<2;++i)
                new_bullet_time->addTimeData(250)
                        .addTimeData(100,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            new_bullet->fadein(1500);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        shoot_small_bullets=false;
        mode=!mode;
    }
    //move
    const int interval=180;
    if(shoot_timer==shoot_cd-200 || shoot_timer==shoot_cd/2-100 || shoot_timer==shoot_cd+10 || shoot_timer==shoot_cd+190) {
        if(player->getX()<0+radius+10) moveTo(0+radius+10,this->y,(shoot_timer==shoot_cd+10 || shoot_timer==shoot_cd+190)?150:220);
        else if(player->getX()>Game::FrameWidth-radius-10) moveTo(Game::FrameWidth-radius-10,this->y,(shoot_timer==shoot_cd+10 || shoot_timer==shoot_cd+190)?150:220);
        else moveTo(player->getX(),this->y,220);
        setVulnerable();
    }
    //thunder
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        double bullet_v, bullet_a;
        int bullet_radius;
        //bullet
        bullet_radius = 8;
        //shoot
        for(int i=0;i<19;++i) {
            bullet_v = 0.2;
            bullet_a = 0.025;
            Bullet_Time* new_bullet_time;
            new_bullet = new_bullet_time = new Bullet_Time(":/res/bullet_yellow.png",bullet_radius,x,y);
            int aim_x, aim_y;
            if(i<=6) {
                aim_x = this->x-i*6;
                aim_y = this->y-(6-i)*20;
            } else if(i>=12) {
                aim_x = this->x+(18-i)*8;
                aim_y = this->y+(i-12)*20;
            } else {
                aim_x = this->x+(i-9)*16;
                aim_y = this->y;
            }
            new_bullet_time->moveTo(aim_x,aim_y,50);
            new_bullet_time->addTimeData(50)
                    .addTimeData(20,0,bullet_v,0,bullet_a);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullet->fadein();
            new_bullets->push_back(new_bullet);
        }
        if(shoot_timer==shoot_cd+interval*2) shoot_timer = 0;
    }
    if(new_bullets->size()>0) return new_bullets;
    return NULL;
}
void Enemy_3_Blue_1::shootSmallBullet() {
    shoot_small_bullets=true;
}
