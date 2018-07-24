#include "effect_shaking.h"

Effect_Shaking::Effect_Shaking(QString img, int img_w, int img_h, int show_w, int show_h, int lifetime, double x, double y, double xv, double yv, double xa, double ya, bool autoFadeoutBeforeDie)
    :Effect(img,img_w,img_h,show_w,show_h,lifetime,x,y,xv,yv,xa,ya,autoFadeoutBeforeDie)
{
    mode=0;
    xc=x;
    yc=y;
}
void Effect_Shaking::move() {
    //move object
    const int shake_time=15;
    switch(mode) {
    case shake_time-1:
        setPosition(x+xv,y+yv);
        setSpeed(xv+xa,yv+ya);
        xc=x;
        yc=y;
        setPosition(x+(qrand()%3-1)*3,y+yv+(qrand()%3-1)*3);
        break;
    case 0:
        x=xc;
        y=yc;
    default:
        xc+=xv;
        yc+=yv;
        setPosition(x+xv,y+yv);
        setSpeed(xv+xa,yv+ya);
        break;
    }
    if(++mode>2*shake_time-1) mode=0;

    //rotate image
    if(rotating) {
        setTransform(QTransform().translate(show_w/2, show_h/2).rotate(0.5*angle++).translate(-show_w/2, -show_h/2));//martrix transform
        if(angle>=360) angle-=360;
    }

    //lifetimer
    if(lifetimer!=-1) --lifetimer; //if lifetime is -1, it won't die
    if(lifetimer==62 && autoFadeoutBeforeDie) fadeout();
    if(lifetimer==0) delete this;
}
