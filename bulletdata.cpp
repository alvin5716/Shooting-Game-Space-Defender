#include "bulletdata.h"

BulletData::BulletData(Bullet* bullet)
    :next_data(NULL), bullet(bullet), infinite(false)
{

}
bool BulletData::isInfinite() const{
    return infinite;
}
void BulletData::setNext(BulletData* next_data) {
    this->next_data = next_data;
}
BulletData* BulletData::next() const {
    return next_data;
}
BulletData::~BulletData() {

}
