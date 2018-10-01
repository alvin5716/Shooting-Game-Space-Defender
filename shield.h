#ifndef SHIELD_H
#define SHIELD_H

#include "effect.h"

class Shield : public Effect
{
public:
    Shield(QString img, int img_w, int img_h, int show_w, int show_h, Character *user, int lifetime=-1, double x=0, double y=0);
    bool floating() const override;
    friend class Enemy;
public slots:
    void move();
private:
    Character* user;
};

#endif // SHIELD_H
