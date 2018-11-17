#include "enemy_4_blue_2.h"
#include "enemy_temp.h"
#include "game.h"

Enemy_4_Blue_2::Enemy_4_Blue_2(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_4_Blue(player,140,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    shoot_count=0;
}
void Enemy_4_Blue_2::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy20_2.png";
        shoot_timer = -530;
        shoot_cd = 10;
        skill_timer = -420;
        emit useSkill("奧伯斯悖論-寰宇之光");
    },
    [this](){
        //skill
        if(skill_timer==-220) moveTo(Game::FrameWidth/2,120,150);
        else if(skill_timer==-20) {
            Effect* bang = new Effect(":/res/bullet_2_yellow.png",50,50,0,0,250,Game::FrameWidth/2,120,0,0,0,0,true);
            bang->zoom(250,800,800);
            bang->rotateStart();
            bang->setOpacity(0.6);
            bang->fadein(500);
            bang->setZValue(-2);
            emit summonEffect(bang);
            emit shakeScreen(static_cast<short>(shakeLevel::largeShake));
        }
        //skill timer
        if(skill_timer<=0) ++skill_timer;
        else Enemy_4_Blue::skill();
    },
    [this](){
        Enemy_4_Blue::skill();
    });
}
std::vector<Bullet*>* Enemy_4_Blue_2::shoot2() {
    const int interval=15;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        setVulnerable();
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        double constexpr bullet_a=0.009;
        double bullet_v=1.2;
        double sin, cos;
        int bullet_radius;
        for(int i=0;i<shoot_times;++i) {
            if(shoot_timer==shoot_cd){
                xp=player->getX();
                yp=player->getY();
                angle[i]=qrand()%100*M_PI/100;
                xd[i] = 40+qrand()%120;
            }
            bullet_v+=i*0.1;
            sin = std::sin(angle[i]);
            cos = std::cos(angle[i]);
            double angle_dif = angle[i]-angleofvector(xp-Game::FrameWidth/2,yp);
            bullet_radius = std::max(14-(int)std::abs(angle_dif*5),8);
            new_bullet = new Bullet(QString(":/res/bullet_yellow.png"),bullet_radius,Game::FrameWidth/2+cos*xd[i],sin*xd[i],bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            new_bullet->rotateAround(xp,yp,bullet_a*std::pow(std::abs(angle_dif),1.8*std::cos(angle_dif)+0.15),angle_dif>0);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullet->fadein();
            new_bullets->push_back(new_bullet);
        }
        if(shoot_timer==shoot_cd+interval*2) shoot_timer = 0;
        return new_bullets;
    }
    return nullptr;
}
