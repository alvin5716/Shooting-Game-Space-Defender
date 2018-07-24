#ifndef EFFECT_SHAKING_H
#define EFFECT_SHAKING_H

#include "effect.h"

class Effect_Shaking: public Effect
{
public:
    Effect_Shaking(QString img, int img_w, int img_h, int show_w, int show_h, int lifetime=250, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool autoFadeoutBeforeDie=false);
public slots:
    virtual void move();
private:
    double xc, yc;
    int mode;
};

#endif // EFFECT_SHAKING_H
