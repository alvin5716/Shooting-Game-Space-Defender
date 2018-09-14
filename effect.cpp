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
}
void Effect::move() {
    //move object
    Character::move();

    //rotate image
    if(rotating) {
        setTransform(QTransform().translate(show_w/2, show_h/2).rotate(0.5*angle++).translate(-show_w/2, -show_h/2));//martrix transform
        if(angle>=720) angle-=720;
    }

    //lifetimer
    if(lifetimer!=-1) --lifetimer; //if lifetime is -1, it won't die
    if(lifetimer==62 && autoFadeoutBeforeDie) fadeout();
    if(lifetimer==0) killItself();
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
