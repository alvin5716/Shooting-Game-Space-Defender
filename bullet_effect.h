#ifndef BULLET_EFFECT_H
#define BULLET_EFFECT_H

#include "effect_attachment.h"
#include "bullet.h"

class Bullet_Effect : public Bullet, public Effect_Attachment
{
public:
    Bullet_Effect(QString img, int radius, QGraphicsScene* scene, QString eff_img, double eff_scale=1.5, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0);
    ~Bullet_Effect();
public slots:
    void move() override;
    void img_move() override;
};

#endif // BULLET_EFFECT_H
