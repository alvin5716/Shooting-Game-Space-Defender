#include "bullet_effect.h"

Bullet_Effect::Bullet_Effect(QString img, int radius, QGraphicsScene* scene, QString eff_img, double eff_scale, double x, double y, double xv, double yv, double xa, double ya)
    :Bullet(img,radius,x,y,xv,yv,xa,ya), Effect_Attachment(scene, eff_img, 120, 120, (int)round(radius*2.2*eff_scale), (int)round(radius*2.2*eff_scale), x, y)
{
    effect->setZValue(Game::ZValueImportantBackEffect);
    effect->setTransformOriginPoint(effect->getShowW()/2,effect->getShowH()/2);
}

void Bullet_Effect::move() {
    Bullet::move();
    this->move_bg(QPointF(this->x,this->y));
}

bool Bullet_Effect::outOfFrame() {
    if(!Bullet::outOfFrame()) return false;
    int r = effect->getShowW()/2;
    return x<0-r || x>Game::FrameWidth+r || y<0-r || y>Game::FrameHeight+r;
}

void Bullet_Effect::img_move() {
    Bullet::img_move();
    this->img_move_bg();
    if(lookForward || rotating) effect->setRotation(this->rotation());
}

Bullet_Effect::~Bullet_Effect() {
    if(effect!=nullptr) delete effect;
}

