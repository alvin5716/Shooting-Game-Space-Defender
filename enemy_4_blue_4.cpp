#include "enemy_4_blue_4.h"
#include "enemy_temp.h"
#include "game.h"
#include <QDebug>

Enemy_4_Blue_4::Enemy_4_Blue_4(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_4_Blue(player,300,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    shoot_count=0;
}
void Enemy_4_Blue_4::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy20_2.png";
        shoot_timer = -500;
        shoot_cd = 20;
        skill_timer = -420;
        emit useSkill("芝諾悖論-飛矢不動");
    },
    [this](){
        //skill
        if(skill_timer==-220) moveTo(player->getX(),120,150);
        //skill timer
        if(++skill_timer>=200) {
            moveTo(player->getX(),120,200);
            skill_timer=0;
        }
    },
    [this](){
        Enemy_4_Blue::skill();
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
            double constexpr bullet_v=3.2;
            if(++shoot_count>5 && bow_count>=10) {
                setVulnerable();
                // arrow center
                sin = std::sin(angle);
                cos = std::cos(angle);
                Enemy* new_enemy = new Enemy_Temp(this,Enemy_Temp::enemy_4_blue_4_shoot,player,17+qrand()%6,qrand()%20,x,y,bullet_v*cos,bullet_v*sin);
                new_enemy->setImg(":/res/bullet_2_purple.png");
                new_enemy->setShowSize(24,24);
                new_enemy->show();
                connect(this,SIGNAL(killItsBullets()),new_enemy,SLOT(killItself()));
                emit summonEnemy(new_enemy);
                // arrow side
                for(int j=1;j<=2;++j) {
                    for(int i=0;i<2;++i) {
                        sin = std::sin(angle);
                        cos = std::cos(angle);
                        new_bullet = new Bullet(QString(":/res/bullet_2_purple.png"),12,x-cos*12*j-sin*(i?1:-1)*9*j,y-sin*12*j+cos*(i?1:-1)*9*j,bullet_v*cos,bullet_v*sin);
                        connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                        new_bullets->push_back(new_bullet);
                    }
                }

                shoot_count=0;
            }
        }
        //bow
        {
            double constexpr bullet_v_2=2.5, bullet_a=-0.025;
            double angle = angleofvector(player->getX()-x,player->getY()-y);
            double sin = std::sin(angle), cos = std::cos(angle);
            double sinv, cosv, sina, cosa;
            for(int i=0;i<2;++i) {
                sinv = std::sin(angle+(i?1:-1)*M_PI/2);
                cosv = std::cos(angle+(i?1:-1)*M_PI/2);
                sina = std::sin(angle);
                cosa = std::cos(angle);
                new_bullet = new Bullet(QString(":/res/bullet_purple.png"),16,x+cos*radius*1.5,y+sin*radius*1.5,bullet_v_2*cosv,bullet_v_2*sinv,bullet_a*cosa,bullet_a*sina);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
            shoot_timer=0;
            if(bow_count<10) ++bow_count;
            return new_bullets;
        }
    }
    return nullptr;
}
