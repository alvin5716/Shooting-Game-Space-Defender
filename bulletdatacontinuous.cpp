#include "bulletdatacontinuous.h"

BulletDataContinuous::BulletDataContinuous(Bullet* bullet, Character* rotate_center, double rotate_ac, bool clockwise)
    :BulletData(bullet), type(BulletDataContinuousType::rotateAroundCharacter)
{
    this->infinite = true;
    this->data.rotateAroundCharacter.rotate_center=rotate_center;
    this->data.rotateAroundCharacter.rotate_ac=rotate_ac;
    this->data.rotateAroundCharacter.clockwise=clockwise;
}
BulletDataContinuous::BulletDataContinuous(Bullet* bullet, int rotate_xc, int rotate_yc, double rotate_ac, bool clockwise)
    :BulletData(bullet), type(BulletDataContinuousType::rotateAroundPoint)
{
    this->infinite = true;
    this->data.rotateAroundPoint.rotate_xc=rotate_xc;
    this->data.rotateAroundPoint.rotate_yc=rotate_yc;
    this->data.rotateAroundPoint.rotate_ac=rotate_ac;
    this->data.rotateAroundPoint.clockwise=clockwise;
}
BulletDataContinuous::BulletDataContinuous(Bullet* bullet, int T, int r, bool sin_or_cos)
    :BulletData(bullet), type(BulletDataContinuousType::moveAsTrigFunction)
{
    this->infinite = true;
    this->data.moveAsTrigFunction.T=T;
    this->data.moveAsTrigFunction.r=r;
    this->data.moveAsTrigFunction.sin_or_cos=sin_or_cos;
    this->data.moveAsTrigFunction.t=0;
    this->data.moveAsTrigFunction.xc=bullet->getX();
    this->data.moveAsTrigFunction.yc=bullet->getY();
}
BulletDataContinuous::BulletDataContinuous(Bullet* bullet, int T, int r, double (*periodic_func)(double))
    :BulletData(bullet), type(BulletDataContinuousType::moveAsPeriodicFunction)
{
    this->infinite = true;
    this->data.moveAsPeriodicFunction.T=T;
    this->data.moveAsPeriodicFunction.r=r;
    this->data.moveAsPeriodicFunction.periodic_func=periodic_func;
    this->data.moveAsPeriodicFunction.t=0;
    this->data.moveAsPeriodicFunction.xc=bullet->getX();
    this->data.moveAsPeriodicFunction.yc=bullet->getY();
}
bool BulletDataContinuous::skill() {
    switch (this->type) {
    case BulletDataContinuousType::rotateAroundCharacter:
    {
        double sin, cos, angle;
        angle = this->bullet->angleofvector(
                    this->data.rotateAroundCharacter.rotate_center->getX()-this->bullet->getX(),
                    this->data.rotateAroundCharacter.rotate_center->getY()-this->bullet->getY());
        angle += ((this->data.rotateAroundCharacter.clockwise)?(-M_PI/2):(M_PI/2));
        cos = std::cos(angle);
        sin = std::sin(angle);
        this->bullet->setAcceleration(this->data.rotateAroundCharacter.rotate_ac*cos,this->data.rotateAroundCharacter.rotate_ac*sin);
        break;
    }
    case BulletDataContinuousType::rotateAroundPoint:
    {
        double sin, cos, angle;
        angle = this->bullet->angleofvector(
                    this->data.rotateAroundPoint.rotate_xc-this->bullet->getX(),
                    this->data.rotateAroundPoint.rotate_yc-this->bullet->getY());
        angle += ((this->data.rotateAroundPoint.clockwise)?(-M_PI/2):(M_PI/2));
        cos = std::cos(angle);
        sin = std::sin(angle);
        this->bullet->setAcceleration(this->data.rotateAroundPoint.rotate_ac*cos,this->data.rotateAroundPoint.rotate_ac*sin);
        break;
    }
    case BulletDataContinuousType::moveAsTrigFunction:
    {
        this->data.moveAsTrigFunction.xc+=this->bullet->xv;
        this->data.moveAsTrigFunction.yc+=this->bullet->yv;
        double angle = M_PI/2+this->bullet->angleofvector(this->bullet->xv,this->bullet->yv),
                dis = this->data.moveAsTrigFunction.r*std::sin(2*M_PI*this->data.moveAsTrigFunction.t/this->data.moveAsTrigFunction.T);
        if(this->data.moveAsTrigFunction.sin_or_cos)
            this->bullet->setPosition(this->data.moveAsTrigFunction.xc+dis*std::cos(angle),this->data.moveAsTrigFunction.yc+dis*std::sin(angle));
        else
            this->bullet->setPosition(this->data.moveAsTrigFunction.xc-dis*std::cos(angle),this->data.moveAsTrigFunction.yc-dis*std::sin(angle));
        if(++this->data.moveAsTrigFunction.t>this->data.moveAsTrigFunction.T) this->data.moveAsTrigFunction.t=0;
        break;
    }
    case BulletDataContinuousType::moveAsPeriodicFunction:
    {
        this->data.moveAsPeriodicFunction.xc+=this->bullet->xv;
        this->data.moveAsPeriodicFunction.yc+=this->bullet->yv;
        double angle = M_PI/2+this->bullet->angleofvector(this->bullet->xv,this->bullet->yv),
                dis = this->data.moveAsPeriodicFunction.r*this->data.moveAsPeriodicFunction.periodic_func(2*M_PI*this->data.moveAsPeriodicFunction.t/this->data.moveAsPeriodicFunction.T);
        this->bullet->setPosition(this->data.moveAsPeriodicFunction.xc+dis*std::cos(angle),this->data.moveAsPeriodicFunction.yc+dis*std::sin(angle));
        if(++this->data.moveAsPeriodicFunction.t>this->data.moveAsPeriodicFunction.T) this->data.moveAsPeriodicFunction.t=0;
        break;
    }
    default:
        break;
    }
    return false;
}
BulletDataContinuous::~BulletDataContinuous()
{

}
