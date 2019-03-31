#include "bulletdata.h"
#include <QDebug>

int BulletData::bulletdata_count = 0;

BulletData::BulletData(Bullet* bullet)
    :next_data(nullptr), bullet(bullet), infinite(false)
{
    ++bulletdata_count;
    //qDebug() << "bullet data count:" << bulletdata_count;
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
    --bulletdata_count;
    //qDebug() << "bullet data count:" << bulletdata_count;
}
