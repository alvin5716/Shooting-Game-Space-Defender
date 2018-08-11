#include "effect.h"

Effect::Effect(QString img, int img_w, int img_h, int show_w, int show_h, int lifetime, double x, double y, double xv, double yv, double xa, double ya, bool autoFadeoutBeforeDie)
    :Character(img, img_w, img_h, show_w, show_h, 1, 0, x, y, xv, yv, xa, ya)
{
    this->autoFadeoutBeforeDie=autoFadeoutBeforeDie;
    rotating=false;
    setInvulnerable();
    this->lifetimer=lifetime;
    //rotate
    angle=0;
}
void Effect::move() {
    //move object
    setPosition(x+xv,y+yv);
    setSpeed(xv+xa,yv+ya);

    //rotate image
    if(rotating) {
        setTransform(QTransform().translate(show_w/2, show_h/2).rotate(0.5*angle++).translate(-show_w/2, -show_h/2));//martrix transform
        if(angle>=720) angle-=720;
    }

    //lifetimer
    if(lifetimer!=-1) --lifetimer; //if lifetime is -1, it won't die
    if(lifetimer==62 && autoFadeoutBeforeDie) fadeout();
    if(lifetimer==0) delete this;
}
void Effect::setVisible() {
    this->setOpacity(1);
}
void Effect::rotateStart() {
    rotating=true;
}
void Effect::killItself() {
    delete this;
}
