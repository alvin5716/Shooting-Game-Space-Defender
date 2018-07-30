#include "enemy_3.h"
#include <QDebug>

Enemy_3::Enemy_3(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    setCanBeMirrored();
}
void Enemy_3::img_move() {
    //move image
    if(img_timer%30==0) {
        if(canBeMirrored) {
            if(face_to_left&&xv>0) face_to_left=false;
            else if((!face_to_left)&&xv<0) face_to_left=true;
        }
        if(img_timer>=30*4) img_timer=0;
        QImage oriImg(img);
        QRect cutFrame(face_to_left?oriImg.width()-img_w*(img_timer/30+1):img_w*(img_timer/30),0,img_w,img_h);
        //mirror image
        QPixmap mirroredImg = QPixmap::fromImage(face_to_left?oriImg.mirrored(true,false):oriImg);
        QPixmap cutImg = mirroredImg.copy(cutFrame);
        setPixmap(cutImg.scaled(show_w,show_h));
    }
    setPos(x-((double)show_w)*(face_to_left?30:24)/54,y-show_h/2);
    ++img_timer;
}
Effect* Enemy_3::disappear() const {
    Effect* corpse = new Effect(img,img_w,img_h,show_w,show_h,disappearTime/8,x+(face_to_left?-1:1)*((double)show_w)/18,y,xv,yv,xa,ya);
    if(canBeMirrored&&face_to_left) {
        corpse->setCanBeMirrored();
        corpse->setFaceToLeft();
    }
    corpse->fadeout(disappearTime);
    return corpse;
}
