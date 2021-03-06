#include "enemy_2_blue_4.h"
#include "laser.h"
#include "bullet_nether.h"

Enemy_2_Blue_4::Enemy_2_Blue_4(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_2_Blue(player,220,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    shoot_count=0;
    is_laser_used=false;
    second_shot=true;
    rainbowSpawnpoint[1]=rainbowSpawnpoint[0]=nullptr;
}
void Enemy_2_Blue_4::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy/2/blue_2.png";
        shoot_timer = 135;
        shoot_cd = 350;
        skill_timer = -75;
        emit useSkill("祖靈的彩虹橋");
    },
    [this](){
        //skill
        if(skill_timer==0) moveTo(Game::FrameWidth/2+80,105,120);
        //skill timer
        if(skill_timer<=0) ++skill_timer;
    });
}
std::vector<Bullet*>* Enemy_2_Blue_4::shoot2() {
    std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
    //rainbow spawnpoint
    if(shoot_timer==150){
        if(player->getY()>240) moveTo(player->getX(),105,120);
        if(is_laser_used){
            Bullet* new_bullet;
            double bullet_v=7;
            switch(shoot_count) {
            case 1:
                for(int i=0;i<2;++i) {
                    rainbowSpawnpoint[i] = new_bullet = new Bullet_Nether(":/res/bullet/2/purple.png",18,nullptr,300,(i==0)?150:0,(i==0)?0:150,bullet_v*std::cos(bullet_angle),bullet_v*sin(bullet_angle));
                    new_bullet->setInvulnerable();
                    connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                    connect(this,SIGNAL(killSpawnpoints()),new_bullet,SLOT(killItself()));
                    new_bullets->push_back(new_bullet);
                }
                break;
            case 2:
                for(int i=0;i<2;++i) {
                    rainbowSpawnpoint[i] = new_bullet = new Bullet_Nether(":/res/bullet/2/purple.png",18,nullptr,300,Game::FrameWidth-((i==0)?0:150),(i==0)?150:0,bullet_v*std::cos(bullet_angle),bullet_v*sin(bullet_angle));
                    new_bullet->setInvulnerable();
                    connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                    connect(this,SIGNAL(killSpawnpoints()),new_bullet,SLOT(killItself()));
                    new_bullets->push_back(new_bullet);
                }
                break;
            }
        }
    //rainbow bullets
    } else if(shoot_timer>150&&shoot_timer<350&&(shoot_timer-150)%3==0) {
        Bullet* new_bullet;
        for(int i=0;i<2;++i) {
            if(rainbowSpawnpoint[i]!=nullptr) {
                for(int j=-3;j<=3;++j) {
                    //fast
                    double bullet_v=5, bullet_a=0.4, sin, cos;
                    cos = ((i==0)?-1:1)*std::cos(bullet_angle+(j+5)*M_PI/10);
                    sin = ((i==0)?-1:1)*std::sin(bullet_angle+(j+5)*M_PI/10);
                    new_bullet = new Bullet(rainbowBullet(j),12,rainbowSpawnpoint[i]->getX(),rainbowSpawnpoint[i]->getY(),bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                    connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                    new_bullets->push_back(new_bullet);
                    //slow
                    if((shoot_timer-150)%33==0) {
                        bullet_v = 0.1;
                        bullet_a = 0.008;
                        cos = std::cos(j*2*M_PI/7);
                        sin = std::sin(j*2*M_PI/7);
                        new_bullet = new Bullet(rainbowBullet(j),8,rainbowSpawnpoint[i]->getX(),rainbowSpawnpoint[i]->getY(),bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                        new_bullet->setVTerminal(1.4);
                        connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                        new_bullets->push_back(new_bullet);
                        new_bullet = new Bullet(rainbowBullet(j),8,rainbowSpawnpoint[i]->getX()+310*((i==0)?-1:1)*std::cos(bullet_angle+M_PI/2),rainbowSpawnpoint[i]->getY()+310*((i==0)?-1:1)*std::sin(bullet_angle+M_PI/2),bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                        new_bullet->setVTerminal(1.4);
                        connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                        new_bullets->push_back(new_bullet);
                    }
                }
            }
        }
    }
    //laser spawnpoint
    if(shoot_timer==shoot_cd-75) {
        const double bullet_v=3, bullet_a=0.12;
        double phi, cos, sin;
        switch(shoot_count) {
        case 0:
        case 2:
            for(int i=0;i<2;++i) {
                phi = angleofvector(((i==0)?0:150)-x,((i==0)?150:0)-radius-y);
                sin = std::sin(phi);
                cos = std::cos(phi);
                stick[i] = new Bullet(":/res/bullet/2/purple.png",22,x,y+radius,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                stick[i]->addWallData();
                stick[i]->setInvulnerable();
                stick[i]->fadein(500);
                connect(this,SIGNAL(killItsBullets()),stick[i],SLOT(killItself()));
                new_bullets->push_back(stick[i]);
            }
            break;
        case 1:
            invulnerable=false;
            for(int i=0;i<2;++i) {
                phi = angleofvector((Game::FrameWidth-((i==0)?0:150))-x,((i==0)?150:0)-radius-y);
                sin = std::sin(phi);
                cos = std::cos(phi);
                stick[i] = new Bullet(":/res/bullet/2/purple.png",22,x,y+radius,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                stick[i]->addWallData();
                stick[i]->setInvulnerable();
                stick[i]->fadein(500);
                connect(this,SIGNAL(killItsBullets()),stick[i],SLOT(killItself()));
                new_bullets->push_back(stick[i]);
            }
        }
    //laser
    } else if(shoot_timer==shoot_cd) {
        emit killSpawnpoints();
        Bullet *new_laser;
        double phi;
        //shoot
        shoot_timer = 0;
        switch(shoot_count) {
        case 0:
            shoot_timer=75;
            shoot_cd=525;
            is_laser_used=true;
        case 2:
            phi = angleofvector(player->getX()-75,player->getY()-75);
            for(int i=0;i<2;++i) {
                double ang = (player->getX()+player->getY()<=150)?-M_PI/5:(phi-M_PI/2+((shoot_count==0)?0:((phi>M_PI/4)?((phi<5*M_PI/16&&player->getY()<Game::FrameHeight/2)?M_PI/15:-M_PI/7):M_PI/6)));
                ang += M_PI/2;
                Laser *new_laser_temp = new Laser(":/res/bullet/laser/purple.png",i==0?this:nullptr,10,ang,0,(shoot_count==0)?233:188,(i==0)?0:150,(i==0)?150:0,(shoot_count==0)?80:150);
                bullet_angle = new_laser_temp->getAngle();
                new_laser = new_laser_temp;
                connect(this,SIGNAL(killItsBullets()),new_laser,SLOT(killItself()));
                connect(new_laser,SIGNAL(deadSignal()),stick[i],SLOT(killItself()));
                new_bullets->push_back(new_laser);
            }
            shoot_count=1;
            break;
        case 1:
            shoot_cd=350;
            phi = angleofvector(player->getX()-Game::FrameWidth+75,player->getY()-75);
            for(int i=0;i<2;++i) {
                double ang = second_shot?phi-M_PI/2:(player->getX()-player->getY()>=Game::FrameWidth-150)?M_PI/5:(phi-M_PI/2+((phi>3*M_PI/4)?-M_PI/6:(phi<11*M_PI/16&&player->getY()<Game::FrameHeight/2)?-M_PI/15:M_PI/7));
                ang += M_PI/2;
                Laser *new_laser_temp = new Laser(":/res/bullet/laser/purple.png",i==0?this:nullptr,10,ang,0,188,Game::FrameWidth-((i==0)?0:150),(i==0)?150:0,150);
                bullet_angle = new_laser_temp->getAngle();
                new_laser = new_laser_temp;
                connect(this,SIGNAL(killItsBullets()),new_laser,SLOT(killItself()));
                connect(new_laser,SIGNAL(deadSignal()),stick[i],SLOT(killItself()));
                new_bullets->push_back(new_laser);
            }
            second_shot=false;
            shoot_count=2;
            break;
        }
    }
    if(new_bullets->size()>0) return new_bullets;
    return nullptr;
}
