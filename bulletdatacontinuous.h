#ifndef BULLETDATACONTINUOUS_H
#define BULLETDATACONTINUOUS_H

#include "bulletdata.h"

enum class BulletDataContinuousType{rotateAroundCharacter, rotateAroundPoint, moveAsTrigFunction, moveAsPeriodicFunction};

class BulletDataContinuous : public BulletData
{
public:
    BulletDataContinuous(Bullet* bullet, Character* rotate_center, double rotate_ac, bool clockwise);
    BulletDataContinuous(Bullet* bullet, int rotate_xc, int rotate_yc, double rotate_ac, bool clockwise);
    BulletDataContinuous(Bullet* bullet, int T, int r, bool sin_or_cos);
    BulletDataContinuous(Bullet* bullet, int T, int r, double (*periodic_func)(double));
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
        struct {
            int T, r, t;
            double xc, yc;
            bool sin_or_cos;
        } moveAsTrigFunction;
        struct {
            int T, r, t;
            double xc, yc;
            double (*periodic_func)(double);
        } moveAsPeriodicFunction;
    } data;
};

#endif // BULLETDATACONTINUOUS_H
