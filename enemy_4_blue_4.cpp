#include "enemy_4_blue_4.h"
#include "enemy_temp.h"
#include "game.h"
#include "enemy_4_yellow.h"
#include <QDebug>

Enemy_4_Blue_4::Enemy_4_Blue_4(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_4_Blue(player,300,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    shoot_count=0;
    bow_count=0;
}
void Enemy_4_Blue_4::skill() {
    double angle_seed = qrand()%10;
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy/4/blue_2.png";
        shoot_timer = -70;
        shoot_cd = 10;
        skill_timer = -210;
        emit useSkill("「飛矢不動悖論」");
        emit killAllBullets();
    },
    [this](){
        //skill
        if(skill_timer==-110) moveTo(player->getX(),120,75);
        else if(skill_timer==-35) emit dialogueStart();
        //skill timer
        if(++skill_timer>=100) {
            moveTo(player->getX(),120,100);
            skill_timer=0;
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
            Enemy_4_Yellow* new_enemy;
            small_enemy = new_enemy = new Enemy_4_Yellow(player,9,40,110,220,this->x,this->y,5*cos,5*sin,-0.08*cos,-0.08*sin,true);
            new_enemy->setInterval(110);
            small_enemy->fadein(1000);
            small_enemy->noPoint();
            connect(this,SIGNAL(useSkill(QString)),small_enemy,SLOT(killItself()));
            connect(small_enemy,SIGNAL(deadSignal()),this,SLOT(small_enemy_died()));
            emit summonEnemy(small_enemy);
        }
    });
}
std::vector<Bullet*>* Enemy_4_Blue_4::shoot2() {
    if(shoot_timer>=shoot_cd) {
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //arrow
        {
            double angle = angleofvector(player->getX()-x,player->getY()-y);
            double sin, cos;
            double constexpr bullet_v=6.4;
            if(++shoot_count>5 && bow_count>=12) {
                setVulnerable();
                // arrow center
                sin = std::sin(angle);
                cos = std::cos(angle);
                Enemy* new_enemy = new Enemy_Temp(this,Enemy_Temp::enemy_4_blue_4_shoot,player,8+qrand()%5,qrand()%10,x,y,bullet_v*cos,bullet_v*sin);
                new_enemy->setImg(":/res/bullet/2/purple.png");
                new_enemy->setShowSize(24,24);
                new_enemy->show();
                connect(this,SIGNAL(killItsBullets()),new_enemy,SLOT(killItself()));
                emit summonEnemy(new_enemy);
                // arrow side
                for(int j=1;j<=2;++j) {
                    for(int i=0;i<2;++i) {
                        sin = std::sin(angle);
                        cos = std::cos(angle);
                        new_bullet = new Bullet(QString(":/res/bullet/2/purple.png"),12,x-cos*12*j-sin*(i?1:-1)*9*j,y-sin*12*j+cos*(i?1:-1)*9*j,bullet_v*cos,bullet_v*sin);
                        connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                        new_bullets->push_back(new_bullet);
                    }
                }

                shoot_count=0;
            }
        }
        //bow
        {
            double constexpr bullet_v_2=5, bullet_a=-0.1;
            double angle = angleofvector(player->getX()-x,player->getY()-y);
            double sin = std::sin(angle), cos = std::cos(angle);
            double sinv, cosv, sina, cosa;
            for(int i=0;i<2;++i) {
                sinv = std::sin(angle+(i?1:-1)*M_PI/2);
                cosv = std::cos(angle+(i?1:-1)*M_PI/2);
                sina = std::sin(angle);
                cosa = std::cos(angle);
                new_bullet = new Bullet(QString(":/res/bullet/1/purple.png"),16,x+cos*radius*1.5,y+sin*radius*1.5,bullet_v_2*cosv,bullet_v_2*sinv,bullet_a*cosa,bullet_a*sina);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
            shoot_timer=0;
            if(bow_count<12) ++bow_count;
            return new_bullets;
        }
    }
    return nullptr;
}
