#include "enemy_4_blue_6.h"
#include "bullet_nether.h"

Enemy_4_Blue_6::Enemy_4_Blue_6(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_4_Blue(player,400,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    this->attackable = false;
    this->point = 100;
    this->setBossHPToSkill();
    setDeathImg(":/res/enemy/4/blue_3.png",181,142);
    setFreezeWhenDied(true);
    setDisappearTime(-1);
    this->shoot_count = -1;
    this->angle = 0;
    this->shoot_timer2 = 0;
    this->shoot_cd2 = 50;
    rng = new SpreadRNG(-20,19);
}
void Enemy_4_Blue_6::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy/4/blue_2.png";
        shoot_timer = -130;
        shoot_cd = 90;
        skill_timer = -360;
        shoot_timer2 = 0;
        shoot_cd2 = 50;
        emit useSkill("「黑洞資訊悖論」");
        emit killAllBullets();
    },
    [this](){
        //skill
        if(skill_timer==-300) moveTo(Game::FrameWidth/2,Game::FrameHeight/2,125);
        else if(skill_timer==-175) {
            emit dialogueStart();
            this->attackable = true;
        } else if(skill_timer==-160) {
            emit shakeScreen(static_cast<short>(shakeLevel::largeShake));
        }
        //skill timer
        if(skill_timer<=0) ++skill_timer;
    });
}
std::vector<Bullet*>* Enemy_4_Blue_6::shoot2() {
    std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
    Bullet* new_bullet;
    if(shoot_timer>=shoot_cd) {
        const int bullet_count = 14, bullet_radius = 16;
        const double bullet_d_min = bullet_radius + std::sqrt(std::pow(Game::FrameWidth/2,2)+std::pow(Game::FrameHeight/2,2));
        if(shoot_count==-1) angle = angleofvector(player->getX()-x,player->getY()-y);
        else angle += M_PI/(bullet_count/2)*0.697;
        for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
            if(shoot_count==-1 && i==0) continue;
            else if(shoot_count==0 && (i==0||i==-1)) continue;
            else if(shoot_count==1 && i==-1) continue;
            for(int j=0;j<3;++j) {
                const double bullet_angle = angle + i*M_PI/(bullet_count/2) + (j-1)*M_PI/35;
                const double cos = std::cos(bullet_angle), sin = std::sin(bullet_angle);
                const double bullet_op=0.9-j*0.15;
                const double bullet_v = -1.5, bullet_a = 0;
                const double bullet_d = bullet_d_min + j*20;
                new_bullet = new Bullet_Nether(QString(":/res/bullet/other/yellow_hex.png"),bullet_radius,this,1000,this->x+bullet_d*cos,this->y+bullet_d*sin,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                new_bullet->setOpacity(bullet_op);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
        if(shoot_count<=5) ++shoot_count;
        shoot_timer = 0;
        if(this->health<=80) shoot_cd = 68;
        else if(this->health<=160) shoot_cd = 78;
        this->setVulnerable();
    }
    if(shoot_count>=5) {
        if(shoot_timer2>=shoot_cd2) {
            double player_angle = angleofvector(player->getX()-x,player->getY()-y);
            double bullet_angle = rng->generate()/40.0*M_PI/5+player_angle;
            const double cos = std::cos(bullet_angle), sin = std::sin(bullet_angle);
            const double bullet_v = 3.5, bullet_a = 0.008, bullet_radius = 12;
            new_bullet = new Bullet(QString(":/res/bullet/4/purple.png"),bullet_radius,this->x,this->y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
            shoot_timer2=0;
            if(this->health<=80) shoot_cd2 = 34;
            else if(this->health<=120) shoot_cd2 = 38;
            else if(this->health<=220) shoot_cd2 = 43;
        }
        ++shoot_timer2;
    }
    if(new_bullets->size()!=0) return new_bullets;
    return nullptr;
}
