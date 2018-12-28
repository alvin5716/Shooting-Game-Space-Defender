#include "enemy_3_blue.h"
#include "bullet_sin.h"
#include "game.h"

Enemy_3_Blue::Enemy_3_Blue(Player* player, int bossSkillHP, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_3(QString(":/res/enemy/3/blue.png"),54,55,radius*2,radius*2,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    point=20;
    mode=false;
    setInvulnerable();
    skill_timer=-100-shoot_cd_init;
    skill_dir = (bool)qrand()%2;
    this->beABoss(bossSkillHP);
    connect(this,SIGNAL(useSkill(QString)),this,SIGNAL(killItsBullets()));
    connect(this,SIGNAL(deadSignal()),this,SIGNAL(killItsBullets()));
    connect(this,SIGNAL(deadSignal()),this,SLOT(deadSet()));
    fadein(250);
}
void Enemy_3_Blue::deadSet() {
    emit deadSignal(x,y);
    emit useSkill("");
}
void Enemy_3_Blue::skill() {
    const int interval = 350;
    if(skill_timer>=0&&skill_timer%interval==0) {
        if(skill_timer>=interval*2) skill_timer=0;
        if((skill_timer==0 && skill_dir) || (skill_timer==interval*1 && !skill_dir)) {
            moveTo(Game::FrameWidth/2+80+qrand()%(Game::FrameWidth/2-350),185+qrand()%50,188);
            show_img_force_set();
        } else if((skill_timer==0 && !skill_dir) || (skill_timer==interval*1 && skill_dir)) {
            moveTo(Game::FrameWidth/2-80-qrand()%(Game::FrameWidth/2-350),185+qrand()%50,188);
            show_img_force_set();
        }
    }
    ++skill_timer;
}
std::vector<Bullet*>* Enemy_3_Blue::shoot() {
    const int interval=15;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        double bullet_v, cos, sin, shoot_angle;
        int bullet_count, t;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a and count
        t = (shoot_timer-shoot_cd)/interval;
        bullet_count = 30;
        if(shoot_timer==shoot_cd) {
            angle = angleofvector(player->getX()-x,player->getY()-y);
            setVulnerable();
        }
        //shoot
        for(int i=-6;i<=6;++i) {
            bullet_v = 2.8+std::abs(i)*0.12;
            if (angle>M_PI/5&&angle<4*M_PI/5) shoot_angle = angle+i*M_PI/(bullet_count/2)+M_PI+t*(mode?1:-1)*M_PI/(bullet_count*2.5);
            else shoot_angle = i*M_PI/(bullet_count/2)-M_PI/2+t*(mode?1:-1)*M_PI/(bullet_count*2.5);
            cos = std::cos(shoot_angle);
            sin = std::sin(shoot_angle);
            new_bullet = new Bullet_Sin(QString(":/res/bullet/1/blue.png"),125,20,8,shootXPos(),shootYPos(),bullet_v*cos,bullet_v*sin,0,0.032);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        if(shoot_timer==shoot_cd+interval*3) {
            mode=!mode;
            shoot_timer = 0;
        }
        return new_bullets;
    }
    return nullptr;
}
void Enemy_3_Blue::redMagicShield() {
    this->showShield(":/res/effect/magic_red.png",120,120);
    shield->setOpacity(0.6);
    shield->fadein(2500);
    shield->setZValue(-2);
    shield->rotateStart();
    emit summonEffect(shield);
}
double Enemy_3_Blue::shootXPos() const{
    return x+(!face_to_left?1:-1)*((double)21/27)*show_w/2;
}
double Enemy_3_Blue::shootYPos() const{
    return y-((double)7.5/27.5)*show_h/2;
}
