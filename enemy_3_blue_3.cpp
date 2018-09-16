#include "enemy_3_blue_3.h"
#include "bullet_nether.h"
#include "bullet_time.h"
#include <QDebug>
#include "game.h"
#include "laser.h"

Enemy_3_Blue_3::Enemy_3_Blue_3(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_3_Blue(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    mode=2;
    clockwise=false;
    shoot_count=0;
    use_laser=true;
}
void Enemy_3_Blue_3::skill() {
    //second phase
    if(health<=230 && !secPhase) {
        secPhase = true;
        invulnerable=true;
        img=":/res/enemy15_2.png";
        shoot_timer = -450;
        shoot_cd = 60;
        skill_timer = -400;
        emit useSkill("阿茲卡班的逃犯");
        this->redMagicShield();
    }
    if(secPhase) {
        //skill
        if(skill_timer==-200) moveTo(Game::FrameWidth/2,120,350);
        //skill timer
        if(skill_timer<=300) {
            ++skill_timer;
            if(skill_timer==150) moveTo(player->getX(),120,125);
        } else skill_timer=0;
    } else {
        Enemy_3_Blue::skill();
    }
}
std::vector<Bullet*>* Enemy_3_Blue_3::shoot2() {
    std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
    Bullet* new_bullet;
    const short interval = 80;
    //center
    if(shoot_timer==-150) {
        new_bullet = center = new Bullet_Time(":/res/bullet_black.png",30,shootXPos(),shootYPos());
        center->addTimeData(150);
        new_bullet->moveTo(Game::FrameWidth/2,Game::FrameHeight/2,150);
        new_bullet->setInvulnerable();
        connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
        new_bullets->push_back(new_bullet);
    //laser
    } else if(use_laser && shoot_timer==30) {
        use_laser=false;
        for(int i=0;i<2;++i) {
            new_bullet = new Laser(":/res/laser_yellow.png",18,M_PI/2+M_PI*mode/5,(i==0)?M_PI/300:-M_PI/300,270,Game::FrameWidth/2,Game::FrameHeight/2,0);
            new_bullet->setZValue(-1);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
    } else if(shoot_timer==shoot_cd) {
        double cos, sin, bullet_d, bullet_a, bullet_v;
        int bullet_radius, bullet_count;
        //circles
        angle = M_PI*mode/5;
        switch (mode) {
        case 0:
            ++mode;
            clockwise=false;
            break;
        case 5:
            --mode;
            clockwise=true;
            break;
        default:
            if(clockwise) --mode;
            else ++mode;
            break;
        }
        //bullet
        bullet_radius = 14;
        bullet_count = 80;
        bullet_v = 0;
        bullet_a = -0.0012;
        bullet_d = std::sqrt(std::pow(Game::FrameHeight/2,2)+std::pow(Game::FrameWidth/2,2))+bullet_radius;
        //shoot
        for(int i=-(bullet_count/2);i<=(bullet_count/2-((bullet_count%2==0)?1:0));++i) {
            if(i<=3 && i>=-3) continue;
            sin = std::sin(angle+i*2*M_PI/bullet_count);
            cos = std::cos(angle+i*2*M_PI/bullet_count);
            Bullet_Nether* new_bullet_nether;
            new_bullet = new_bullet_nether = new Bullet_Nether(":/res/bullet_yellow.png",bullet_radius,center,1000,center->getX()+bullet_d*cos,center->getY()+bullet_d*sin,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            new_bullet_nether->setFadeoutTime(100);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
    } else if(shoot_timer>shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        setVulnerable();
        double cos, sin, bullet_a, bullet_v;
        int bullet_radius, bullet_count, t;
        t=(shoot_timer-shoot_cd)/interval;
        //purple random bullets
        if(shoot_count<6) ++shoot_count;
        else {
            //bullet
            bullet_radius = 6;
            bullet_count = 22;
            bullet_a = 0.0005;
            //shoot
            for(int i=-(bullet_count/2);i<=(bullet_count/2-((bullet_count%2==0)?1:0));++i) {
                double rand_angle = angle+(i+t*0.5+(double)(qrand()%30)/30)*2*M_PI/bullet_count;
                bullet_v = 0.13+qrand()%15/100;
                sin = std::sin(rand_angle);
                cos = std::cos(rand_angle);
                new_bullet = new Bullet(":/res/bullet_purple.png",bullet_radius,center->getX(),center->getY(),bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
        if(shoot_timer==shoot_cd+interval*3) {
            shoot_timer=0;
            switch (mode) {
            case 0:
            case 5:
                shoot_cd = 200;
                break;
            default:
                shoot_cd = 60;
                break;
            }
        }
    }
    if(new_bullets->size()>0) return new_bullets;
    return NULL;
}
