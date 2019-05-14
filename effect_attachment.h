#ifndef BULLET_BACKGROUND_H
#define BULLET_BACKGROUND_H

#include "effect.h"
#include "bullet.h"

class Effect_Attachment
{
public:
    Effect_Attachment(QGraphicsScene* scene, QString eff_img, int img_w, int img_h, int show_w, int show_h, double x, double y);
    void move_bg(QPointF pos);
    void img_move_bg();
    Effect* getEffect();
protected:
    Effect* effect;
};

#endif // BULLET_BACKGROUND_H
