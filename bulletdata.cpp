#include "bulletdata.h"

BulletData::BulletData(Character* bullet)
    :next_data(NULL), bullet(bullet)
{

}
void BulletData::setNext(BulletData* next_data) {
    this->next_data = next_data;
}
BulletData* BulletData::next() const {
    return next_data;
}
BulletData::~BulletData() {

}
