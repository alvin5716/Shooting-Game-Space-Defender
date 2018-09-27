#include "enemy_3.h"
#include <QDebug>

Enemy_3::Enemy_3(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    setCanBeMirrored();
}
void Enemy_3::img_move() {
    show_img_set();
    if(floating) setPos(x-((double)show_w)*(face_to_left?30:24)/54,y-show_h/2+Character::float_distance*std::sin((double)float_timer/125*M_PI));
    else setPos(x-((double)show_w)*(face_to_left?30:24)/54,y-show_h/2);
}
Effect* Enemy_3::disappear() const {
    Effect* corpse = new Effect((death_img=="")?img:death_img,img_w,img_h,show_w,show_h,disappearTime/8,x+(face_to_left?-1:1)*((double)show_w)/18,y+Character::float_distance*std::sin((double)float_timer/125*M_PI),xv,yv,xa,ya);
    if(canBeMirrored&&face_to_left) {
        corpse->setCanBeMirrored();
        corpse->setFaceToLeft();
    }
    if(floating) corpse->setFloating(true);
    corpse->fadeout(disappearTime);
    return corpse;
}
