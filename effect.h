#ifndef EFFECT_H
#define EFFECT_H

#include "character.h"

class Effect : public Character
{
public:
    Effect(QString img, int img_w, int img_h, int show_w, int show_h, int lifetime=250, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool autoFadeoutBeforeDie=false);
    void rotateStart();
public slots:
    virtual void move();
protected:
    int lifetimer;
    unsigned int angle;
    bool rotating, autoFadeoutBeforeDie;
};

#endif // EFFECT_H
