#ifndef BULLET_BACKGROUND_H
#define BULLET_BACKGROUND_H

#include "effect.h"
#include "bullet.h"

class Effect_Attachment
{
public:
    Effect_Attachment(QGraphicsScene* scene, const QString &eff_img, int img_w, int img_h, int show_w, int show_h, double x, double y);
    Effect_Attachment(QGraphicsScene* scene, int eff_pixmap, int img_w, int img_h, int show_w, int show_h, double x, double y);
    void move_bg(QPointF pos);
    void img_move_bg();
    Effect* getEffect();
    void setEffectImgSize(int w, int h);
protected:
    Effect* effect;
};

#endif // BULLET_BACKGROUND_H
