#include "enemy_3_blue_2.h"
#include "bullet_distance.h"
#include <QDebug>
#include "game.h"

Enemy_3_Blue_2::Enemy_3_Blue_2(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_3_Blue(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    room=NULL;
    mode=false;
    isBulletFaster=false;
}
void Enemy_3_Blue_2::skill() {
    //second phase
    if(health<=190 && !secPhase) {
        secPhase = true;
        invulnerable=true;
        img=":/res/enemy15_2.png";
        shoot_timer = -500;
        shoot_cd = 190;
        skill_timer = -200;
        emit useSkill("消失的密室");
    }
    if(secPhase) {
        //skill
        if(skill_timer==0) moveTo(Game::FrameWidth/2,140,200);
        //skill timer
        if(skill_timer<=200) ++skill_timer;
    } else {
        Enemy_3_Blue::skill();
    }
}
std::vector<Bullet*>* Enemy_3_Blue_2::shoot2() {
    const int room_radius = 200;
    if(skill_timer>=200) {
        if(room==NULL) {
            room = new Effect(":/res/shield2.png",70,70,room_radius*2,room_radius*2,-1,player->getX(),player->getY());
            room_radius_rate=0;
            room->setTransform(QTransform().scale(0.01,0.01));
            connect(this,SIGNAL(deadSignal()),room,SLOT(killItself()));
            emit summonEffect(room);
        } else {
            room->setPosition(player->getX(),player->getY());
            if(room_radius_rate<125) {
                ++room_radius_rate;
                room->setTransform(QTransform().translate(room_radius-room_radius*((double)room_radius_rate/125),room_radius-room_radius*((double)room_radius_rate/125)).scale(((double)room_radius_rate/125),((double)room_radius_rate/125)));
            }
        }
    }
    std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
    Bullet* new_bullet;
    const int interval=20;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        setVulnerable();
        int t = (shoot_timer-shoot_cd)/interval;
        double bullet_v, bullet_a, cos, sin;
        int bullet_radius, bullet_count;
        if(!isBulletFaster) {
            //bullet
            bullet_radius = 8;
            bullet_count = 20;
            bullet_v = 0.6;
            bullet_a = 0.005;
            if(t==0) {
                angle = (2*M_PI/bullet_count)*(qrand()%10)/10;
                omega_seed = 5+qrand()%11;
                omega = (mode?1:-1)*(2*M_PI/bullet_count)*omega_seed/30;
            }
            else angle += omega;
            //shoot
            for(int i=-(bullet_count/2);i<=(bullet_count/2-((bullet_count%2==0)?1:0));++i) {
                sin = std::sin(angle+i*2*M_PI/bullet_count);
                cos = std::cos(angle+i*2*M_PI/bullet_count);
                Bullet_Distance* new_bullet_distance;
                new_bullet = new_bullet_distance = new Bullet_Distance(":/res/bullet_purple.png",bullet_radius,player,room_radius,x,y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                connect(new_bullet_distance,&Bullet_Distance::enterArea,[=](){ new_bullet_distance->fadeout(10); });
                connect(new_bullet_distance,&Bullet_Distance::leaveArea,[=](){ new_bullet_distance->fadein(10); });
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
            if(t==6) {
                shoot_cd = 140;
                shoot_timer=0;
                isBulletFaster=true;
            }
        } else {
            //bullet
            bullet_radius = 25;
            bullet_count = 14;
            bullet_v = 0;
            bullet_a = 0.018;
            if(t==7) {
                angle = (2*M_PI/bullet_count)*(qrand()%10)/10;
                omega_seed = 20-omega_seed;
                omega = (mode?1:-1)*(2*M_PI/bullet_count)*omega_seed/30;
            }
            else angle += omega;
            //shoot
            for(int i=-(bullet_count/2);i<=(bullet_count/2-((bullet_count%2==0)?1:0));++i) {
                sin = std::sin(angle+i*2*M_PI/bullet_count);
                cos = std::cos(angle+i*2*M_PI/bullet_count);
                Bullet_Distance* new_bullet_distance;
                new_bullet = new_bullet_distance = new Bullet_Distance(":/res/bullet_black.png",bullet_radius,player,room_radius,x,y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                connect(new_bullet_distance,&Bullet_Distance::enterArea,[=](){ new_bullet_distance->fadeout(10); });
                connect(new_bullet_distance,&Bullet_Distance::leaveArea,[=](){ new_bullet_distance->fadein(10); });
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
            if(t==4) {
                shoot_cd = 340;
                shoot_timer=0;
                isBulletFaster=false;
                mode=!mode;
            }
        }
    }
    if(new_bullets->size()>0) return new_bullets;
    return NULL;
}
