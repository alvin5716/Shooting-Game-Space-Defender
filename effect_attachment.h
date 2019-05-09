#ifndef BULLET_BACKGROUND_H
#define BULLET_BACKGROUND_H

#include "effect.h"
#include "bullet.h"

class Effect_Attachment
{
public:
    Effect_Attachment(QGraphicsScene* scene, QString eff_img, int show_w, int show_h, double x, double y);
    void move_bg(QPointF pos);
    void img_move_bg();
protected:
    Effect* background;
};

#endif // BULLET_BACKGROUND_H
