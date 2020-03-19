#include "enemy_3_blue_2.h"
#include "bullet_distance.h"
#include <QDebug>
#include "game.h"

Enemy_3_Blue_2::Enemy_3_Blue_2(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_3_Blue(player,180,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    room=nullptr;
    mode=false;
    isBulletFaster=false;
    shoot_count=-1;
}
void Enemy_3_Blue_2::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy/3/blue_2.png";
        shoot_timer = -450;
        shoot_cd = 220;
        skill_timer = -100;
        emit useSkill("消失的密室");
        this->redMagicShield();
    },
    [this](){
        //skill
        if(skill_timer==0) moveTo(Game::FrameWidth/2,140,100);
        //skill timer
        if(skill_timer<=100) ++skill_timer;
        //invisible room
        if(skill_timer>=100) {
            if(room==nullptr) {
                room = new Effect(":/res/effect/magic_red.png",120,120,1,1,-1,player->getX(),player->getY());
                room->setOpacity(0.6);
                room->setZValue(Game::ZValueBackEffect);
                room->rotateStart();
                room->zoom(room_radius*2,room_radius*2,63);
                connect(this,SIGNAL(deadSignal()),room,SLOT(killItself()));
                emit summonEffect(room);
            } else {
                room->setPosition(player->getX(),player->getY());
            }
        }
    },
    [this](){
        Enemy_3_Blue::skill();
    });
}
std::vector<Bullet*>* Enemy_3_Blue_2::shoot2() {
    std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
    Bullet* new_bullet;
    const int interval=10;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        setVulnerable();
        int t = (shoot_timer-shoot_cd)/interval;
        double bullet_v, bullet_a, cos, sin;
        int bullet_radius, bullet_count;
        //slower
        if(!isBulletFaster) {
            bullet_radius = 8;
            bullet_count = 18;
            bullet_v = 1.2;
            bullet_a = 0.02;
            switch (t) {
            case 0:
                angle = (2*M_PI/bullet_count)*(qrand()%10)/10;
                switch (shoot_count) {
                case -1:
                    omega=0;
                    break;
                case 0:
                    omega_seed = 3+qrand()%3;
                    omega = (mode?1:-1)*(2*M_PI/bullet_count)*omega_seed/30;
                    break;
                case 1:
                    omega_seed = 8+qrand()%3;
                    omega = (mode?1:-1)*(2*M_PI/bullet_count)*omega_seed/30;
                    break;
                default:
                    omega_seed = 6+qrand()%3;
                    omega = (mode?1:-1)*(2*M_PI/bullet_count)*omega_seed/30;
                    break;
                }
                break;
            case 4:
                if(shoot_count>=2) omega*=-1;
            default:
                angle += omega;
                break;
            }
            //shoot
            for(int i=-(bullet_count/2);i<=(bullet_count/2-((bullet_count%2==0)?1:0));++i) {
                sin = std::sin(angle+i*2*M_PI/bullet_count);
                cos = std::cos(angle+i*2*M_PI/bullet_count);
                Bullet_Distance* new_bullet_distance;
                new_bullet = new_bullet_distance = new Bullet_Distance(":/res/bullet/3/purple.png",bullet_radius,player,room_radius,shootXPos(),shootYPos());
                new_bullet->rotateStart();
                int dis_stop = (8-t)*15;
                new_bullet->moveTo(shootXPos()+dis_stop*cos,shootYPos()+dis_stop*sin,60);
                new_bullet->addTimeData(60) //move to initial pos
                        ->addTimeData(5,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin); //shoot
                connect(new_bullet_distance,&Bullet_Distance::enterArea,[=](){ new_bullet_distance->fadeout(150); });
                connect(new_bullet_distance,&Bullet_Distance::leaveArea,[=](){ new_bullet_distance->fadein(150); });
                connect(player,SIGNAL(healthChanged(int)),new_bullet_distance,SLOT(disable()));
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
            if(t==6) {
                shoot_cd = 70;
                shoot_timer = 0;
                isBulletFaster=true;
            }
        //bullet faster
        } else {
            bullet_radius = 25;
            bullet_count = 14;
            bullet_v = 0;
            bullet_a = 0.072;
            switch(t) {
            case 0:
                angle = (2*M_PI/bullet_count)*(qrand()%10)/10;
                switch (shoot_count) {
                case -1:
                    omega=0;
                    break;
                case 0:
                    omega_seed = 3+qrand()%3;
                    omega = (mode?1:-1)*(2*M_PI/bullet_count)*omega_seed/30;
                    break;
                case 1:
                    omega_seed = 8+qrand()%3;
                    omega = (mode?1:-1)*(2*M_PI/bullet_count)*omega_seed/30;
                    break;
                default:
                    omega_seed = 6+qrand()%3;
                    omega = (mode?1:-1)*(2*M_PI/bullet_count)*omega_seed/30;
                    break;
                }
                break;
            default:
                angle += omega;
                break;
            }
            //shoot
            for(int i=-(bullet_count/2);i<=(bullet_count/2-((bullet_count%2==0)?1:0));++i) {
                sin = std::sin(angle+i*2*M_PI/bullet_count);
                cos = std::cos(angle+i*2*M_PI/bullet_count);
                Bullet_Distance* new_bullet_distance;
                new_bullet = new_bullet_distance = new Bullet_Distance(":/res/bullet/3/red.png",10,player,room_radius,shootXPos(),shootYPos());
                new_bullet->rotateStart();
                int dis_stop = (5-t)*20;
                new_bullet->moveTo(shootXPos()+dis_stop*cos,shootYPos()+dis_stop*sin,60);
                new_bullet->addTimeData(60) //move to initial pos
                        ->addTimeData(5,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin) //shoot
                        ->addTimeData(50,bullet_radius); //zoom
                connect(new_bullet_distance,&Bullet_Distance::enterArea,[=](){ new_bullet_distance->fadeout(100); });
                connect(new_bullet_distance,&Bullet_Distance::leaveArea,[=](){ new_bullet_distance->fadein(100); });
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                connect(player,SIGNAL(healthChanged(int)),new_bullet_distance,SLOT(disable()));
                new_bullets->push_back(new_bullet);
            }
            if(t==4) {
                if(++shoot_count>2) shoot_count=0;
                moveTo(Game::FrameWidth/2+(qrand()%121-60),140+(qrand()%41-20),100);
                shoot_cd = 200;
                shoot_timer = 80;
                isBulletFaster=false;
                mode=!mode;
            }
        }
    //yellow bullets
    } else if(shoot_timer==-138||shoot_timer==-75||shoot_timer==-13) {
        double bullet_v, bullet_a, cos, sin;
        //bullet v, a and count
        bullet_a = 0.048;
        angle = angleofvector(player->getX()-shootXPos(),player->getY()-shootYPos());
        for(int i=0;i<2;++i) {
            for(int j=0;j<8;++j) {
                bullet_v = 1+j*0.6;
                sin = std::sin(angle+((i==0)?1:-1)*M_PI/20);
                cos = std::cos(angle+((i==0)?1:-1)*M_PI/20);
                Bullet_Distance* new_bullet_distance;
                new_bullet = new_bullet_distance = new Bullet_Distance(":/res/bullet/3/yellow.png",10,player,room_radius,shootXPos(),shootYPos(),bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                new_bullet->rotateStart();
                connect(new_bullet_distance,&Bullet_Distance::enterArea,[=](){
                    new_bullet_distance->setOpacity(0.3);
                });
                connect(new_bullet_distance,&Bullet_Distance::leaveArea,[=](){
                    new_bullet_distance->setOpacity(1);
                });
                connect(player,SIGNAL(healthChanged(int)),new_bullet_distance,SLOT(disable()));
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
        if(shoot_timer==-13) shoot_timer=80;
    }
    if(new_bullets->size()>0) return new_bullets;
    return nullptr;
}
