#ifndef EFFECT_H
#define EFFECT_H

#include "character.h"

class Effect : public Character
{
    Q_OBJECT
public:
    Effect(QString img, int img_w, int img_h, int show_w, int show_h, int lifetime=250, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool autoFadeoutBeforeDie=false);
    void rotateStart();
    int getLifeTime() const;
    void zoom(int aim_show_w, int aim_show_h, int time);
    friend class Enemy;
public slots:
    virtual void move();
    void killItself();
protected:
    int lifetimer, zoom_timer;
    unsigned int angle;
    bool rotating, autoFadeoutBeforeDie;
    double increment_w, increment_h, now_show_w, now_show_h;
};

#endif // EFFECT_H
