#include "bullet.h"
#include "bulletdatatime.h"
#include "bulletdatawall.h"
#include <cmath>
#include <QDebug>

Bullet::Bullet(QString img, int radius, double x, double y, double xv, double yv, double xa, double ya)
    :Character(img,50,50,(int)round(radius*2.2),(int)round(radius*2.2),1,radius,x,y,xv,yv,xa,ya)
{
    terminal_v = 0;
    this->data_head = NULL;
}

void Bullet::setVTerminal(double terminal_v) {
    this->terminal_v = terminal_v;
}

void Bullet::move() {
    if(this->data_head!=NULL && this->data_head->skill()) {
        if(!this->data_head->isInfinite()) {
            BulletData* temp = this->data_head;
            this->data_head = temp->next();
            delete temp;
        }
        emit triggered();
    }
    if(terminal_v==0) {
        Character::move();
    } else {
        //move object
        setPosition(x+xv,y+yv);
        if((sqrt(pow(xv,2)+pow(yv,2))<=terminal_v && sqrt(pow(xv+xa,2)+pow(yv+ya,2))<=terminal_v) || (sqrt(pow(xv,2)+pow(yv,2))>=terminal_v && sqrt(pow(xv+xa,2)+pow(yv+ya,2))>=terminal_v)) setSpeed(xv+xa,yv+ya);
    }
}
void Bullet::addData(BulletData* bullet_data) {
    if(this->data_head==NULL) {
        this->data_head = bullet_data;
        return;
    }
    BulletData* ptr=this->data_head;
    while(ptr->next()!=NULL) ptr=ptr->next();
    ptr->setNext(bullet_data);
    bullet_data->setNext(NULL);
}
// bullet time data
Bullet* Bullet::addTimeData(int wait_time) {
    BulletData* bullet_data = new BulletDataTime(this,wait_time);
    this->addData(bullet_data);
    return this;
}
Bullet* Bullet::addTimeData(int wait_time, int aim_radius) {
    BulletData* bullet_data = new BulletDataTime(this,wait_time,aim_radius);
    this->addData(bullet_data);
    return this;
}
Bullet* Bullet::addTimeData(int wait_time, double xv, double yv, double xa, double ya) {
    BulletData* bullet_data = new BulletDataTime(this,wait_time,xv,yv,xa,ya);
    this->addData(bullet_data);
    return this;
}
Bullet* Bullet::addTimeData(int wait_time, Character* player, double v, double a) {
    BulletData* bullet_data = new BulletDataTime(this,wait_time,player,v,a);
    this->addData(bullet_data);
    return this;
}
Bullet* Bullet::addTimeData(int wait_time, double x, double y, int time) {
    BulletData* bullet_data = new BulletDataTime(this,wait_time,x,y,time);
    this->addData(bullet_data);
    return this;
}
// bullet wall data
Bullet* Bullet::addWallData() {
    BulletData* bullet_data = new BulletDataWall(this);
    this->addData(bullet_data);
    return this;
}
Bullet* Bullet::addWallData(bool infinite_bounce, bool bounce_when_hit_downside) {
    BulletData* bullet_data = new BulletDataWall(this,infinite_bounce, bounce_when_hit_downside);
    this->addData(bullet_data);
    return this;
}
Bullet* Bullet::addWallData(double vertical_v, double vertical_a) {
    BulletData* bullet_data = new BulletDataWall(this,vertical_v,vertical_a);
    this->addData(bullet_data);
    return this;
}
Bullet* Bullet::addWallData(Character* player, double v) {
    BulletData* bullet_data = new BulletDataWall(this,player,v);
    this->addData(bullet_data);
    return this;
}
Bullet* Bullet::addWallData(Character* player, double v, double a) {
    BulletData* bullet_data = new BulletDataWall(this,player,v,a);
    this->addData(bullet_data);
    return this;
}
