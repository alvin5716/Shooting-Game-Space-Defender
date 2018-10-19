#include "enemy_4_blue.h"
#include "game.h"

Enemy_4_Blue::Enemy_4_Blue(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_4(QString(":/res/enemy20.png"),199,153,std::round(3.902*radius),3*radius,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    point=18;
    boss=true;
    invulnerable=true;
    connect(this,SIGNAL(useSkill(QString)),this,SIGNAL(killItsBullets()));
    connect(this,SIGNAL(deadSignal()),this,SIGNAL(killItsBullets()));
    connect(this,SIGNAL(deadSignal()),this,SLOT(deadSet()));
    skill_timer=-250;
}

void Enemy_4_Blue::skill() {
    const int interval = 350;
    if(skill_timer>=interval) {
        skill_timer=0;
        double aim_x, aim_y;
        if(x<240 || x>Game::FrameWidth-240 || y>350 || y<150) {
            if(y>350) aim_y=350;
            else if(y<150) aim_y=150;
            else {
                int yd=qrand()%121-60;
                if(y+yd>350 || y+yd<150) aim_y=y-yd;
                else aim_y=y+yd;
            }
            if(x>Game::FrameWidth-240) aim_x=Game::FrameWidth-240;
            else if(x<240) aim_x=240;
            else if(x<player->getX()+radius+20 && x>player->getX()-radius-20) {
                int xd=(x>player->getX()?1:-1)*(qrand()%21+70);
                if(x+xd<240 || x+xd>Game::FrameWidth-240) aim_x=x-xd;
                else aim_x=x+xd;
            } else {
                aim_x=x;
            }
        } else if(std::sqrt(std::pow(x-player->getX(),2)+std::pow(y-player->getY(),2))<radius+140) {
            //too near
            double ang = angleofvector(player->getX()-x,player->getY()-y);
            double sin = std::sin(ang);
            double cos = std::cos(ang);
            aim_x=x+cos*90;
            aim_y=y+sin*90;
        } else if(x<player->getX()+radius+20 && x>player->getX()-radius-20) {
            int xd=(x>player->getX()?1:-1)*(qrand()%21+70), yd=qrand()%121-60;
            //x out of range
            if(x+xd<240 || x+xd>Game::FrameWidth-240) aim_x=x-xd;
            else aim_x=x+xd;
            //y out of range
            if(y+yd>350 || y+yd<150) aim_y=y-yd;
            else aim_y=y+yd;
        } else {
            aim_x=x;
            aim_y=y;
        }
        //move
        moveTo(aim_x,aim_y,125);
    }
    ++skill_timer;
}

void Enemy_4_Blue::deadSet() {
    emit deadSignal(x,y);
    emit useSkill("");
}

std::vector<Bullet*>* Enemy_4_Blue::shoot() {
    constexpr int interval=35, shoot_count=4;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        const int r=12;
        const int t=(shoot_timer-shoot_cd)/interval;
        if(t==0) {
            // shoot angle
            const int w=Game::FrameWidth-r;
            const double x1=player->getX(), y1=player->getY(), phi=M_PI*(qrand()%5)/100;
            double k;
            k=((x-r)*y1+(x1-r)*y)/(x+x1-2*r);
            angle[0]=angleofvector(r-x,k-y)+phi;
            k=((w-x)*y1+(w-x1)*y)/(2*w-x-x1);
            angle[1]=angleofvector(w-x,k-y)+phi;
            // attack position
            attack_x=x;
            attack_y=y;
            // vulnerable
            setVulnerable();
        }
        double sin, cos, sin_x, cos_x;
        constexpr double bullet_v=1.7;
        constexpr int bullet_xd=20;
        for(int i=0;i<2;++i) {
            for(int j=0;j<2;++j) {
                sin = std::sin(angle[j]);
                cos = std::cos(angle[j]);
                sin_x = std::sin(angle[j]+(i?M_PI/2:-M_PI/2));
                cos_x = std::cos(angle[j]+(i?M_PI/2:-M_PI/2));
                new_bullet = new Bullet(QString(":/res/bullet_blue.png"),r,attack_x+cos*radius/2+cos_x*bullet_xd*t,attack_y+sin*radius/2+sin_x*bullet_xd*t,bullet_v*cos,bullet_v*sin);
                new_bullet->addWallData(false,false);
                new_bullet->fadein();
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
        if(t>=shoot_count) shoot_timer=0;
        return new_bullets;
    }
    return nullptr;
}
