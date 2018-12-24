#include "enemy_blue_2.h"
#include <QDebug>

Enemy_Blue_2::Enemy_Blue_2(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_Blue(player,130,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    clockwise=false;
}
void Enemy_Blue_2::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy/1/blue_2.png";
        shoot_timer = -88;
        shoot_cd = 125;
        skill_timer = -50;
        emit useSkill("螺旋星雲");
    },
    [this](){
        //skill
        if(skill_timer==0) moveTo(Game::FrameWidth/2,330,125);
        //skill timer
        if(skill_timer<=125) ++skill_timer;
    },
    [this](){
        if(skill_timer==165) emit dialogueStart();
        if(skill_timer<=165) ++skill_timer;
    });
}
std::vector<Bullet*>* Enemy_Blue_2::shoot2() {
    const int total_t = 7;
    const int interval = 63;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        double bullet_v, cos, sin;
        int bullet_count, t;
        std::vector<Bullet*>* new_bullets = new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a
        t = (shoot_timer-shoot_cd)/interval;
        bullet_v = 0.8;
        bullet_count = 24;
        if(shoot_timer==shoot_cd) {
            invulnerable=false;
            angle=angleofvector(player->getX()-x,player->getY()-y);
            clockwise=!clockwise;
        }
        //shoot
        //black balls
        if(t==2) {
            for(int i=0;i<2;++i) {
                new_bullet = new Bullet(QString(":/res/bullet/1/black.png"),35,(i==0)?220:Game::FrameWidth-220,-34,0,4.4,0,-0.04);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
        //rotate bullets
        if(t<4) {
            if(t==0) invulnerable=false;
            for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
                int bullet_radius=((i+(bullet_count/2))%4)*5+12;
                cos = std::cos(angle+2*i*M_PI/bullet_count+(M_PI*8*t/bullet_count/3));
                sin = std::sin(angle+2*i*M_PI/bullet_count+(M_PI*8*t/bullet_count/3));
                new_bullet = new Bullet(QString(":/res/bullet/1/purple.png"),bullet_radius,x+(double)radius*2/3*cos,y+(double)radius*2/3*sin,bullet_v*cos,bullet_v*sin);
                new_bullet->rotateAround(x,y,0.04,clockwise);
                new_bullet->fadein(500);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        } else if(t==4) {
            for(int j=0;j<2;++j) {
                for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
                    cos = std::cos(angle+2*i*M_PI/bullet_count+M_PI/bullet_count);
                    sin = std::sin(angle+2*i*M_PI/bullet_count+M_PI/bullet_count);
                    new_bullet = new Bullet(QString(":/res/bullet/1/purple.png"),12,(j==0)?220:Game::FrameWidth-220,200,bullet_v*cos,bullet_v*sin);
                    new_bullet->rotateAround((j==0)?220:Game::FrameWidth-220,200,0.028,clockwise);
                    connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                    new_bullets->push_back(new_bullet);
                }
            }
        }
        if(shoot_timer==shoot_cd+interval*(total_t-1)) shoot_timer = 0;
        return new_bullets;
    }
    return nullptr;
}
