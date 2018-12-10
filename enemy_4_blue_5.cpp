#include "enemy_4_blue_5.h"
#include "game.h"
#include <cfloat>

Enemy_4_Blue_5::Enemy_4_Blue_5(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_4_Blue(player,200,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    point+=10;
    death_img=":/res/enemy/4/blue.png";
    setDisappearTime(5000);
}
void Enemy_4_Blue_5::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy/4/blue_2.png";
        shoot_timer = -150;
        shoot_cd = 120;
        skill_timer = -210;
        emit useSkill("遠距傳送悖論");
    },
    [this](){
        //skill
        if(skill_timer==-110) moveTo(Game::FrameWidth/2,Game::FrameHeight/2-100,125);
        //skill timer
        if(skill_timer<=0) ++skill_timer;
        else {
            setSpeed(player->getX()>this->x?DBL_MIN:-DBL_MIN,0);
            this->show_img_force_set();
        }
    },
    [this](){
        Enemy_4_Blue::skill();
    });
}
std::vector<Bullet*>* Enemy_4_Blue_5::shoot2() {
    auto single_shoot = [this](std::vector<Bullet*>* new_bullets, bool snipe)->void{
        Bullet* new_bullet;
        const int bullet_count=8;
        double sin, cos;
        const double angle=angleofvector(player->getX()-x,player->getY()-y) + (snipe?0:M_PI/bullet_count);
        const int bullet_radius = 10;
        std::vector<QPoint> shoot_points = {
            QPoint(14,-9), //head
            QPoint(-12,27), //body 1
            QPoint(16,27), //body 2
            QPoint(56,11), //wing 1
            QPoint(-58,14) //wing 2
        };
        for(QPoint shoot_point: shoot_points) {
            const int rand_seed = qrand()%8;
            const double bullet_a = 0.003+rand_seed/8000.0, bullet_v_t = 1.8+rand_seed/10.0;
            for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
                double shoot_ang = angle+(i+qrand()%50/50.0)*M_PI/(bullet_count/2);
                cos = std::cos(shoot_ang);
                sin = std::sin(shoot_ang);
                new_bullet = new Bullet(QString(":/res/bullet/1/purple.png"),bullet_radius,shoot_point.x()*(face_to_left?-1:1)+this->x+bullet_radius*2*cos,shoot_point.y()+this->y+bullet_radius*2*sin,0,0,bullet_a*cos,bullet_a*sin);
                new_bullet->fadein(500);
                new_bullet->setVTerminal(bullet_v_t);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
    };
    if(shoot_timer==shoot_cd) {
        //shoot
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        single_shoot(new_bullets, true);
        //move
        this->fadeout(500);
        //return
        return new_bullets;
    } if(shoot_timer>=shoot_cd+31) {
        //shoot
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        single_shoot(new_bullets, false);
        //move
        setVulnerable();
        double distance = std::sqrt(std::pow(this->x-player->getX(),2) + std::pow(this->y-player->getY(),2));
        if(distance > max_distance) {
            double angle=angleofvector(player->getX()-x,player->getY()-y);
            double cos = std::cos(angle), sin = std::sin(angle);
            setPosition(this->x+max_distance*cos,this->y+max_distance*sin);
        } else setPosition(player->getX(),player->getY());
        this->fadein(500);
        //cd change
        if(shoot_cd==120 && health<=150) shoot_cd=100;
        else if(shoot_cd==100 && health<=100) shoot_cd=85;
        else if(shoot_cd==85 && health<=50) shoot_cd=70;
        //return
        shoot_timer=0;
        return new_bullets;
    }
    return nullptr;
}
