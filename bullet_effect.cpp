#include "bullet_effect.h"

Bullet_Effect::Bullet_Effect(QString img, int radius, QGraphicsScene* scene, QString eff_img, double eff_scale, double x, double y, double xv, double yv, double xa, double ya)
    :Bullet(img,radius,x,y,xv,yv,xa,ya), Effect_Attachment(scene, eff_img, (int)round(radius*2.2*eff_scale), (int)round(radius*2.2*eff_scale), x, y)
{
    this->setZValue(Game::ZValueImportantBackEffect);
}

void Bullet_Effect::move() {
    Bullet::move();
    this->move_bg(QPointF(this->x,this->y));
}
void Bullet_Effect::img_move() {
    Bullet::img_move();
    this->img_move_bg();
}
Bullet_Effect::~Bullet_Effect() {
    if(background!=nullptr) delete background;
}

