#ifndef BULLET_COLLECTION_H
#define BULLET_COLLECTION_H

#include "bullet.h"

class Bullet_Collection : public Bullet
{
public:
    Bullet_Collection(int bullet_count, const QString &img, int radius, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0);
    void addBullet(int x=0, int y=0, int xv=0, int yv=0, int xa=0, int ya=0);
public slots:
    void move();
private:
    std::vector<Bullet*> psas;
};

#endif // BULLET_COLLECTION_H
