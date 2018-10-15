#include "enemy_4_blue.h"
#include "game.h"

Enemy_4_Blue::Enemy_4_Blue(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_4(QString(":/res/enemy20.png"),199,153,std::round(3.902*radius),3*radius,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    point=12;
    boss=true;
    invulnerable=true;
    connect(this,SIGNAL(useSkill(QString)),this,SIGNAL(killItsBullets()));
    connect(this,SIGNAL(deadSignal()),this,SIGNAL(killItsBullets()));
    connect(this,SIGNAL(deadSignal()),this,SLOT(deadSet()));
}

void Enemy_4_Blue::skill() {
    if(prep_timer>0) {
        --prep_timer;
        return;
    }
    setVulnerable();
    return;
}

void Enemy_4_Blue::deadSet() {
    emit deadSignal(x,y);
    emit useSkill("");
}

std::vector<Bullet*>* Enemy_4_Blue::shoot() {
    if(prep_timer>0) return nullptr;
    if(shoot_timer>=shoot_cd) {
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        const int r=12;
        const double x1=player->getX(), y1=player->getY(), w=Game::FrameWidth+r;
        const double k[2]={((r+x)*y1+(r+x1)*y)/(2*r+x+x1),((w-x)*y1+(w-x1)*y)/(2*w-x-x1)}, angle[2]={angleofvector(-x,k[0]-y),angleofvector(w-x,k[1]-y)};
        double sin, cos, bullet_v=1.2;
        for(int i=0;i<2;++i) {
            sin = std::sin(angle[i]);
            cos = std::cos(angle[i]);
            new_bullet = new Bullet(QString(":/res/bullet_2_blue.png"),r,x,y,bullet_v*cos,bullet_v*sin);
            new_bullet->addWallData(false,false);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        shoot_timer=0;
        return new_bullets;
    }
    return nullptr;
}
