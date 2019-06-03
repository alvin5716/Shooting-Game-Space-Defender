#include "enemy_green.h"

Enemy_Green::Enemy_Green(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy(ImgPixmap::Level1::enemy_1_green,40,37,std::round(radius*2.162),radius*2,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    point=2;
}
