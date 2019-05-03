#include "enemy_4.h"

Enemy_4::Enemy_4(QString img, int img_w, int img_h, int show_w, int show_h, Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable),
    invulnerable_after_init(false)
{
    setCanBeMirrored();
    max_speed = 1;
    prep_timer = 75;
}
void Enemy_4::setInvulnerable() {
    this->invulnerable_after_init = true;
}
