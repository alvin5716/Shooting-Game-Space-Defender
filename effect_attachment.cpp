#include "effect_attachment.h"

Effect_Attachment::Effect_Attachment(QGraphicsScene* scene, QString eff_img, int show_w, int show_h, double x, double y)
    :background(new Effect(eff_img, 50, 50, show_w, show_h,-1,x,y))
{
    if(scene!=nullptr) scene->addItem(this->background);
    this->background->setZValue(Game::ZValueBackEffect);
}

void Effect_Attachment::move_bg(QPointF pos) {
    this->background->setPosition(pos);
    this->background->move();
}

void Effect_Attachment::img_move_bg() {
    this->background->img_move();
}
