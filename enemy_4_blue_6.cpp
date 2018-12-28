#include "enemy_4_blue_6.h"
#include "bullet_nether.h"

Enemy_4_Blue_6::Enemy_4_Blue_6(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_4_Blue(player,300,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable), ice_shooting(false)
{
    this->attackable = false;
    this->point = 50;
    this->setBossHPToSkill();
    setDeathImg(":/res/enemy/4/blue_3.png",181,142);
    setDisappearTime(-1);
}
void Enemy_4_Blue_6::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy/4/blue_2.png";
        shoot_timer = -130;
        shoot_cd = 120;
        skill_timer = -360;
        emit useSkill("「熱力學-復冰」");
        emit killAllBullets();
    },
    [this](){
        //skill
        if(skill_timer==-300) moveTo(Game::FrameWidth/2,120,125);
        else if(skill_timer==-175) {
            emit dialogueStart();
            this->attackable = true;
        }
        //skill timer
        if(skill_timer<=0) ++skill_timer;
        else Enemy_4_Blue::skill();
    });
}
std::vector<Bullet*>* Enemy_4_Blue_6::shoot2() {
    if(shoot_timer>=shoot_cd) {
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        if(ice_shooting) { //ice
            setVulnerable();
            const int bullet_radius = 55, bullet_count = 10;
            const double bullet_v = 1.2;
            int x0 = -qrand()%(Game::FrameWidth/bullet_count);
            for(int i=0;i<=bullet_count+1;++i) {
                Bullet_Ice* new_bullet_ice;
                new_bullet = new_bullet_ice = new Bullet_Ice(QString(":/res/bullet/other/ice.png"),bullet_radius,nullptr,100,x0+i*(Game::FrameWidth/bullet_count),bullet_radius*3/8.0,0,0,0,0.05);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                connect(this,SIGNAL(newCenter(Character*)),new_bullet_ice,SLOT(addNewCenter(Character*)));
                new_bullet->fadein();
                new_bullet->setVTerminal(bullet_v);
                new_bullets->push_back(new_bullet);
            }
        }
        ice_shooting = !ice_shooting;
        { //yellow ball
            const double angle = angleofvector(player->getX()-x,player->getY()-y);
            const double sin = std::sin(angle), cos = std::cos(angle);
            const int bullet_radius = 70;
            const double bullet_v = 4.2;
            new_bullet = new Bullet_Nether(QString(":/res/bullet/other/big.png"),bullet_radius+50,nullptr,1000,x,y,bullet_v*cos,bullet_v*sin);
            new_bullet->addTimeData(30,bullet_radius);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullet->setOpacity(0.9);
            new_bullet->fadein(500);
            new_bullet->setInvulnerable();
            emit newCenter(new_bullet);
            new_bullets->push_back(new_bullet);
        }
        shoot_timer=0;
        shoot_cd = 120-std::min(300-health,250)/4.4;
        return new_bullets;
    }
    return nullptr;
}

Bullet_Ice::Bullet_Ice(QString img, int radius, Character* center, int distance, double x,double y,double xv,double yv,double xa,double ya)
    :Bullet_Distance(img,radius,center,distance,x,y,xv,yv,xa,ya)
{
    int max_radius = radius;
    connect(this,&Bullet_Ice::enterArea,[this,max_radius](){
        this->clearData();
        this->addTimeData((int)std::round(30.0*(this->radius-5)/(max_radius-5)),5);
    });
    connect(this,&Bullet_Ice::leaveArea,[this,max_radius](){
        this->clearData();
        this->addTimeData((int)std::round(300.0*(max_radius-this->radius)/(max_radius-5)),max_radius);
    });
}
