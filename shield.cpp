#include "shield.h"

Shield::Shield(const QString &img, int img_w, int img_h, int show_w, int show_h, Character *user, int lifetime, double x, double y)
    :Effect(img, img_w, img_h, show_w, show_h, lifetime, x, y, 0, 0, 0, 0, true), user(user)
{
    this->setZValue(Game::ZValueForeEffect);
}
void Shield::move() {
    Effect::move();
    setPosition(user->getX(),user->getY());
}
bool Shield::floating() const{
    return user->floating();
}
