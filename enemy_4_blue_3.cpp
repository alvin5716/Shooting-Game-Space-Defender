#include "enemy_4_blue_3.h"
#include "enemy_temp.h"
#include "enemy_4_red.h"
#include "game.h"
#include <QDebug>

Enemy_4_Blue_3::Enemy_4_Blue_3(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_4_Blue(player,180,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    fall_count=0;
    connect(this,&Enemy_4_Blue_3::bouncedAtY,[this](){
        emit shakeScreenVertical(static_cast<short>(shakeLevel::largeShake));
        this->dust_falling = true;
        setVulnerable();
        if(fall_count<2) ++fall_count;
    });
    this->purple_shooting=false;
    this->dust_falling=false;
}
void Enemy_4_Blue_3::skill() {
    double angle_seed = qrand()%10;
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy/4/blue_2.png";
        shoot_timer = 0;
        shoot_cd = 23;
        skill_timer = -210;
        emit useSkill("「懦夫賽局」");
        emit killAllBullets();
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
            else if(std::abs(this->xv)>0.15){
                if (this->xv>0) this->xa=-0.06;
                else if (this->xv<0) this->xa=0.06;
            } else {
                this->xa=0.0;
                this->xv = 0;
            }

            if(this->xv>2.4) this->xv=2.4;
            else if(this->xv<-2.4) this->xv=-2.4;

            if(this->y<120) {
                this->y=120;
                this->yv=0;
            }
        }
    },
    [this,angle_seed](){
        Enemy_4_Blue::skill();
        if(small_enemy_timer>0) --small_enemy_timer;
        else if(small_enemy==nullptr || small_enemy->isDead()) {
            double angle = ((angle_seed-4.5)/10.0)/5*M_PI;
            angle += (angle>0?M_PI/3:-M_PI/3) - M_PI/2;
            double cos = std::cos(angle);
            double sin = std::sin(angle);
            small_enemy = new Enemy_4_Red(player,6,40,75,150,this->x,this->y,5*cos,5*sin,-0.08*cos,-0.08*sin);
            small_enemy->fadein(1000);
            small_enemy->noPoint();
            connect(this,SIGNAL(useSkill(QString)),small_enemy,SLOT(killItself()));
            connect(small_enemy,SIGNAL(deadSignal()),this,SLOT(small_enemy_died()));
            emit summonEnemy(small_enemy);
        }
    });
}
std::vector<Bullet*>* Enemy_4_Blue_3::shoot2() {
    if(shoot_timer>=shoot_cd) {
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //black falling dust
        const int bullet_count = 15;
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
        //purple
        if(purple_shooting) {
            double const bullet_v=4.4;
            double sin, cos;
            for(int i=0;i<2;++i) {
                double angle = M_PI/2+(i?1:-1)*M_PI/5;
                sin = std::sin(angle);
                cos = std::cos(angle);
                new_bullet = new Bullet(QString(":/res/bullet/1/purple.png"),14,x+cos*radius,y+sin*radius,bullet_v*cos,bullet_v*sin);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
        if(fall_count>=2 && !purple_shooting) {
            purple_shooting=true;
            shoot_timer=-100;
        } else shoot_timer=0;
        return new_bullets;
    }
    return nullptr;
}
