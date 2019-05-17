#include "effect_attachment.h"

Effect_Attachment::Effect_Attachment(QGraphicsScene* scene, const QString &eff_img, int img_w, int img_h, int show_w, int show_h, double x, double y)
    :effect(new Effect(eff_img, img_w, img_h, show_w, show_h,-1,x,y))
{
    if(scene!=nullptr) scene->addItem(this->effect);
}

void Effect_Attachment::move_bg(QPointF pos) {
    this->effect->setPosition(pos);
    this->effect->move();
}

void Effect_Attachment::img_move_bg() {
    this->effect->img_move();
}

Effect* Effect_Attachment::getEffect() {
    return effect;
}
