#include "enemy_3.h"
#include <QDebug>

Enemy_3::Enemy_3(QString img, int img_w, int img_h, int show_w, int show_h, Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    setCanBeMirrored();
}
double Enemy_3::imgX() const{
    return x-((double)show_w)*(face_to_left?30:24)/54;
}
