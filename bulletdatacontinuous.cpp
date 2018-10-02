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
    default:
        break;
    }
    return false;
}
BulletDataContinuous::~BulletDataContinuous()
{

}
