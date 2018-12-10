#include "enemy_4_blue_3.h"
#include "enemy_temp.h"
#include "game.h"
#include <QDebug>

Enemy_4_Blue_3::Enemy_4_Blue_3(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_4_Blue(player,180,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    shoot_count=0;
    connect(this,&Enemy_4_Blue_3::bouncedAtY,[this](){
        emit shakeScreenVertical(static_cast<short>(shakeLevel::largeShake));
        this->dust_falling = true;
        setVulnerable();
    });
    this->dust_falling=false;
}
void Enemy_4_Blue_3::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy/4/blue_2.png";
        shoot_timer = -135;
        shoot_cd = 23;
        skill_timer = -210;
        emit useSkill("膽小鬼賽局");
    },
    [this](){
        //skill
        if(skill_timer==-110) moveTo(Game::FrameWidth/2,120,75);
        //skill timer
        if(skill_timer<0) {
            ++skill_timer;
        } else if(skill_timer==0) {
            ++skill_timer;
            this->bounceable = true;
            this->stopable = false;
            this->setAcceleration(0,0.032);
        } else {
            if(player->getX()>this->x+this->radius) this->xa=0.04;
            else if(player->getX()<this->x-this->radius) this->xa=-0.04;
            else if(this->x<player->getX()+5 && this->x>player->getX()-5) {
                this->xv=0;
                this->xa=0;
            } else if (this->xv>0) this->xa=-0.06;
            else if (this->xv<0) this->xa=0.06;
            else this->xa=0.0;

            if(this->xv>2.4) this->xv=2.4;
            else if(this->xv<-2.4) this->xv=-2.4;

            if(this->y<120) {
                this->y=120;
                this->yv=0;
            }
        }
    },
    [this](){
        Enemy_4_Blue::skill();
    });
}
std::vector<Bullet*>* Enemy_4_Blue_3::shoot2() {
    if(shoot_timer>=shoot_cd) {
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //purple
        double constexpr bullet_v=4.4;
        double sin, cos;
        for(int i=0;i<2;++i) {
            double angle = M_PI/2+(i?1:-1)*M_PI/5;
            sin = std::sin(angle);
            cos = std::cos(angle);
            new_bullet = new Bullet(QString(":/res/bullet/1/purple.png"),14,x+cos*radius,y+sin*radius,bullet_v*cos,bullet_v*sin);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        //black falling dust
        constexpr int bullet_count = 15;
        if(dust_falling) {
            int x0 = (int)std::round(player->getX())%bullet_count;
            for(int i=0;i<=bullet_count;++i) {
                new_bullet = new Bullet(QString(":/res/bullet/1/black.png"),10,x0+i*(Game::FrameWidth/bullet_count),qrand()%17-8,0,qrand()%10/30.0*2,0,0.032);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullet->fadein();
                new_bullets->push_back(new_bullet);
            }
            dust_falling=false;
        }
        shoot_timer=0;
        return new_bullets;
    }
    return nullptr;
}
