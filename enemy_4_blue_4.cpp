#include "enemy_4_blue_4.h"
#include "enemy_temp.h"
#include "game.h"

Enemy_4_Blue_4::Enemy_4_Blue_4(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_4_Blue(player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{

}
void Enemy_4_Blue_4::skill() {
    //second phase
    if(health<=180 && !secPhase) {
        secPhase = true;
        invulnerable=true;
        img=":/res/enemy20_2.png";
        shoot_timer = -350;
        shoot_cd = 160;
        skill_timer = -200;
        emit useSkill("芝諾悖論-飛矢不動");
    }
    if(secPhase) {
        //skill
        if(skill_timer==0) moveTo(Game::FrameWidth/2,120,200);
        //skill timer
        if(skill_timer<=0) ++skill_timer;
    } else {
        Enemy_4_Blue::skill();
    }
}
std::vector<Bullet*>* Enemy_4_Blue_4::shoot2() {
    if(shoot_timer>=shoot_cd) {
        setVulnerable();
        double angle = angleofvector(player->getX()-x,player->getY()-y);
        double sin = std::sin(angle), cos = std::cos(angle);
        double constexpr bullet_v=3.2;
        Enemy* new_enemy = new Enemy_Temp(Enemy_Temp::enemy_4_blue_4_shoot,player,40,qrand()%40,x,y,bullet_v*cos,bullet_v*sin);
        new_enemy->setImg(":/res/bullet_purple.png");
        new_enemy->setShowSize(20,20);
        emit summonEnemy(new_enemy);
        shoot_timer=0;
    }
    return nullptr;
}
