#include "effect.h"
#include <QDebug>

Effect::Effect(QString img, int img_w, int img_h, int show_w, int show_h, int lifetime, double x, double y, double xv, double yv, double xa, double ya, bool autoFadeoutBeforeDie)
    :Character(img, img_w, img_h, show_w, show_h, 1, 0, x, y, xv, yv, xa, ya)
{
    this->autoFadeoutBeforeDie=autoFadeoutBeforeDie;
    setInvulnerable();
    this->lifetimer=lifetime;
    //rotate
    rotating=false;
    angle=0;
    //zoom
    increment_w=increment_h=0;
    now_show_w=show_w;
    now_show_h=show_h;
}
void Effect::move() {
    //move object
    Character::move();

    //rotate image
    if(rotating) {
        setTransform(QTransform().translate(show_w/2, show_h/2).rotate(angle++).translate(-show_w/2, -show_h/2));//martrix transform
        if(angle>=360) angle-=360;
    }

    //lifetimer
    if(lifetimer!=-1) { //if lifetime is -1, it won't die
        --lifetimer;
        if(lifetimer<=0) killItself();
    }
    if(lifetimer==31 && autoFadeoutBeforeDie) fadeout();

    //zoom
    if(zoom_timer>0) {
        --zoom_timer;
        this->now_show_w += this->increment_w;
        this->show_w = (int)this->now_show_w;
        this->now_show_h += this->increment_h;
        this->show_h = (int)this->now_show_h;
        this->show_img_force_set();
    }
}
void Effect::zoom(int aim_show_w, int aim_show_h, int time) {
    increment_w = ((double)aim_show_w-show_w)/time;
    increment_h = ((double)aim_show_h-show_h)/time;
    zoom_timer = time;
    now_show_w = show_w;
    now_show_h = show_h;
}
void Effect::rotateStart() {
    rotating=true;
}
void Effect::killItself() {
    delete this;
}
int Effect::getLifeTime() const{
    return lifetimer;
}
