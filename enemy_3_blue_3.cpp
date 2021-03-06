#include "enemy_3_blue_3.h"
#include "bullet_nether.h"
#include <QDebug>
#include "game.h"
#include "laser.h"

Enemy_3_Blue_3::Enemy_3_Blue_3(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_3_Blue(player,230,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    shoot_count=0;
    use_laser=true;
    shoot_count_yellow = 0;
}
void Enemy_3_Blue_3::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable = true;
        img=":/res/enemy/3/blue_2.png";
        shoot_timer = -270;
        shoot_cd = 30;
        skill_timer = -200;
        emit useSkill("魔法監獄的逃犯");
        this->redMagicShield();
    },
    [this](){
        //skill
        if(skill_timer==-100) moveTo(Game::FrameWidth/2,80,175);
        //skill timer
        if(skill_timer<=150) {
            ++skill_timer;
            if(skill_timer==75) {
                moveTo(player->getX(),80,63);
            }
        } else skill_timer=0;
    },
    [this](){
        Enemy_3_Blue::skill();
    });
}
std::vector<Bullet*>* Enemy_3_Blue_3::shoot2() {
    std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
    Bullet* new_bullet;
    const short interval = 60;
    //center
    if(shoot_timer==-75) {
        //black
        new_bullet = center = new Bullet(":/res/bullet/1/purple.png",35,shootXPos(),shootYPos());
        new_bullet->addTimeData(75);
        new_bullet->moveTo(Game::FrameWidth/2,Game::FrameHeight/2,75);
        new_bullet->setInvulnerable();
        connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
        new_bullets->push_back(new_bullet);
        //purple rotate
        new_bullet = new Bullet(":/res/bullet/3/purple.png",25,shootXPos(),shootYPos());
        new_bullet->addTimeData(75);
        new_bullet->moveTo(Game::FrameWidth/2,Game::FrameHeight/2,75);
        new_bullet->setInvulnerable();
        connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
        new_bullets->push_back(new_bullet);
    //laser
    } else if(use_laser && shoot_timer==15) {
        use_laser=false;
        for(int i=0;i<2;++i) {
            new_bullet = new Laser(":/res/bullet/laser/yellow.png",i==0?this:nullptr,18,M_PI+(2+shoot_count_yellow)*M_PI/5,(i==0)?M_PI/200:-M_PI/200,180,Game::FrameWidth/2,Game::FrameHeight/2,0);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
    } else if(shoot_timer==shoot_cd) {
        double cos, sin, bullet_d, bullet_a, bullet_v, angle;
        int bullet_radius, bullet_count;
        //circles
        angle = (2+shoot_count_yellow)*M_PI/5;
        if(++shoot_count_yellow>=10) shoot_count_yellow=0;
        //bullet
        bullet_radius = 14;
        bullet_count = 80;
        bullet_v = 0;
        bullet_a = -0.0048;
        bullet_d = std::sqrt(std::pow(Game::FrameHeight/2,2)+std::pow(Game::FrameWidth/2,2))+bullet_radius;
        //shoot
        for(int i=-(bullet_count/2);i<=(bullet_count/2-((bullet_count%2==0)?1:0));++i) {
            if(i<=3 && i>=-4) continue;
            sin = std::sin(angle+i*2*M_PI/bullet_count);
            cos = std::cos(angle+i*2*M_PI/bullet_count);
            Bullet_Nether* new_bullet_nether;
            new_bullet = new_bullet_nether = new Bullet_Nether(":/res/bullet/3/yellow.png",bullet_radius,center,1000,center->getX()+bullet_d*cos,center->getY()+bullet_d*sin,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            new_bullet_nether->setFadeoutTime(100);
            new_bullet->moveAsTrigFunction(100,bullet_radius,true);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
    } else if(shoot_timer>shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        setVulnerable();
        double cos, sin, bullet_a, bullet_v;
        int bullet_radius, bullet_count, t;
        t=(shoot_timer-shoot_cd)/interval;
        //black random bullets
        if(shoot_count<6) ++shoot_count;
        else {
            //bullet
            bullet_radius = 6;
            bullet_count = 16;
            bullet_a = 0.002;
            //shoot
            for(int i=-(bullet_count/2);i<=(bullet_count/2-((bullet_count%2==0)?1:0));++i) {
                double rand_angle = (i+t*0.33+(double)(qrand()%30)/30)*2*M_PI/bullet_count;
                bullet_v = 0.18+(double)(qrand()%17)/100;
                sin = std::sin(rand_angle);
                cos = std::cos(rand_angle);
                new_bullet = new Bullet(":/res/bullet/1/black.png",bullet_radius,center->getX()+bullet_radius*2*cos,center->getY()+bullet_radius*2*sin,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                new_bullet->fadein();
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
        if(shoot_timer==shoot_cd+interval*2) shoot_timer=0;
    }
    if(new_bullets->size()>0) return new_bullets;
    return nullptr;
}
