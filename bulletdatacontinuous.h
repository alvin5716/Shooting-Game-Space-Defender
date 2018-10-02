#ifndef BULLETDATACONTINUOUS_H
#define BULLETDATACONTINUOUS_H

#include "bulletdata.h"

enum class BulletDataContinuousType{rotateAroundCharacter, rotateAroundPoint};

class BulletDataContinuous : public BulletData
{
public:
    BulletDataContinuous(Bullet* bullet, Character* rotate_center, double rotate_ac, bool clockwise);
    BulletDataContinuous(Bullet* bullet, int rotate_xc, int rotate_yc, double rotate_ac, bool clockwise);
    bool skill() override;
    ~BulletDataContinuous() override;
private:
    BulletDataContinuousType type;
    union {
        struct {
            Character* rotate_center;
            double rotate_ac;
            bool clockwise;
        } rotateAroundCharacter;
        struct {
            int rotate_xc, rotate_yc;
            double rotate_ac;
            bool clockwise;
        } rotateAroundPoint;
    } data;
};

#endif // BULLETDATACONTINUOUS_H
