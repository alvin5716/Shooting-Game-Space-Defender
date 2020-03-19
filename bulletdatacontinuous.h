#ifndef BULLETDATACONTINUOUS_H
#define BULLETDATACONTINUOUS_H

#include "bulletdata.h"

enum class BulletDataContinuousType{rotateAroundCharacter, rotateAroundPoint, gravityFromCharacter, gravityFromPoint, moveAsTrigFunction, moveAsPeriodicFunction};

class BulletDataContinuous : public BulletData
{
public:
    BulletDataContinuous(Bullet* bullet, Character* rotate_center, double rotate_ac, bool clockwise);
    BulletDataContinuous(Bullet* bullet, int rotate_xc, int rotate_yc, double rotate_ac, bool clockwise);
    BulletDataContinuous(Bullet* bullet, Character* gravity_center, double gravity_ac);
    BulletDataContinuous(Bullet* bullet, int gravity_xc, int gravity_yc, double gravity_ac);
    BulletDataContinuous(Bullet* bullet, int T, int r, bool sin_or_cos, int phase);
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
            Character* gravity_center;
            double gravity_ac;
        } gravityFromCharacter;
        struct {
            int gravity_xc, gravity_yc;
            double gravity_ac;
        } gravityFromPoint;
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
