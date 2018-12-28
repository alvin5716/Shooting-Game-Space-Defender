#ifndef BULLETDATA_H
#define BULLETDATA_H

#include "bullet.h"
#include "player.h"

class BulletData
{
public:
    BulletData(Bullet* bullet);
    bool isInfinite() const;
    BulletData* next() const;
    void setNext(BulletData* next_data);
    virtual bool skill() = 0;
    virtual ~BulletData();
protected:
    BulletData* next_data;
    Bullet* bullet;
    bool infinite;
private:
    static int bulletdata_count;
};

#endif // BULLETDATA_H
