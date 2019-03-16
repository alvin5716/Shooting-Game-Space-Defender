#include "enemy_4_blue_6.h"
#include "bullet_nether.h"

Enemy_4_Blue_6::Enemy_4_Blue_6(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_4_Blue(player,300,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
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
        skill_timer = -260;
        emit useSkill("「黑洞資訊悖論」");
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
    });
}
std::vector<Bullet*>* Enemy_4_Blue_6::shoot2() {
    if(shoot_timer>=shoot_cd) {

    }
    return nullptr;
}
