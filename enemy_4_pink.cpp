#include "enemy_4_pink.h"
#include "enemy_temp.h"
#include <QDebug>

Enemy_4_Pink::Enemy_4_Pink(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
:Enemy_4(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    move_speed=1.6;
}

void Enemy_4_Pink::attacked() {
    Character::attacked();
    if(health<=0) {
        Enemy* new_enemy = new Enemy_Temp(Enemy_Temp::enemy_4_pink_shoot,player,10,10,x,y);
        emit summonEnemy(new_enemy);
    }
}
