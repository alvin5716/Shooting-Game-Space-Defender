#include "shield.h"

Shield::Shield(QString img, int img_w, int img_h, int show_w, int show_h, Character *user, int lifetime, double x, double y)
    :Effect(img, img_w, img_h, show_w, show_h, lifetime, x, y, 0, 0, 0, 0, true), user(user)
{

}
void Shield::move() {
    //move
    setPosition(user->getX(),user->getY());

    //lifetimer
    if(lifetimer!=-1) --lifetimer; //if lifetime is -1, it won't die
    if(lifetimer==62) fadeout();
    if(lifetimer==0) delete this;
}
