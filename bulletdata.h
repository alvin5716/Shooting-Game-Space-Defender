#ifndef BULLETDATA_H
#define BULLETDATA_H

#include "character.h"

class BulletData
{
public:
    BulletData(Character* bullet);
    BulletData* next() const;
    void setNext(BulletData* next_data);
    virtual bool skill() = 0;
    virtual ~BulletData();
protected:
    BulletData* next_data;
    Character* bullet;
};

#endif // BULLETDATA_H
