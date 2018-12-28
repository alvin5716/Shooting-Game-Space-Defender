#include "bulletdatatime.h"
#include "bullet.h"
#include <QDebug>

BulletDataTime::BulletDataTime(Bullet* bullet, int wait_time)
    :BulletData(bullet), wait_time(wait_time), type(BulletDataTimeType::freeze)
{

}
BulletDataTime::BulletDataTime(Bullet* bullet, int wait_time, int aim_radius)
    :BulletData(bullet), wait_time(wait_time), type(BulletDataTimeType::zoom)
{
    this->data.zoom.aim_radius=aim_radius;
    this->data.zoom.now_radius=bullet->getRadius();
    this->data.zoom.increment=((double)aim_radius-bullet->getRadius())/wait_time;
}
BulletDataTime::BulletDataTime(Bullet* bullet, int wait_time, double xv, double yv, double xa, double ya)
    :BulletData(bullet), wait_time(wait_time), type(BulletDataTimeType::updateVA)
{
    this->data.updateVA.xv=xv;
    this->data.updateVA.yv=yv;
    this->data.updateVA.xa=xa;
    this->data.updateVA.ya=ya;
}
BulletDataTime::BulletDataTime(Bullet* bullet, int wait_time, Player* player, double v, double a)
    :BulletData(bullet), wait_time(wait_time), type(BulletDataTimeType::shootAtPlayer)
{
    this->data.shootAtPlayer.player=player;
    this->data.shootAtPlayer.v=v;
    this->data.shootAtPlayer.a=a;
}
BulletDataTime::BulletDataTime(Bullet* bullet, int wait_time, double x, double y, int time)
    :BulletData(bullet), wait_time(wait_time), type(BulletDataTimeType::moveTo)
{
    this->data.moveTo.x=x;
    this->data.moveTo.y=y;
    this->data.moveTo.time=time;
}

bool BulletDataTime::skill() {
    switch (this->type) {
    case BulletDataTimeType::zoom:
        this->data.zoom.now_radius += this->data.zoom.increment;
        this->bullet->radius = (int)this->data.zoom.now_radius;
        this->bullet->show_w = this->bullet->show_h = (int)round(this->bullet->radius*2.2);
        this->bullet->show_img_force_set();
        break;
    default:
        break;
    }
    if(--this->wait_time<=0) {
        switch (this->type) {
        case BulletDataTimeType::freeze:
            this->bullet->setSpeed(0,0);
            this->bullet->setAcceleration(0,0);
            break;
        case BulletDataTimeType::updateVA:
            this->bullet->setSpeed(this->data.updateVA.xv,this->data.updateVA.yv);
            this->bullet->setAcceleration(this->data.updateVA.xa,this->data.updateVA.ya);
            break;
        case BulletDataTimeType::shootAtPlayer:
        {
            double angle = this->bullet->angleofvector(
                        this->data.shootAtPlayer.player->getX()-this->bullet->getX(),
                        this->data.shootAtPlayer.player->getY()-this->bullet->getY());
            double sin = std::sin(angle);
            double cos = std::cos(angle);
            this->bullet->setSpeed(this->data.shootAtPlayer.v*cos,this->data.shootAtPlayer.v*sin);
            this->bullet->setAcceleration(this->data.shootAtPlayer.a*cos,this->data.shootAtPlayer.a*sin);
            break;
        }
        case BulletDataTimeType::moveTo:
            this->bullet->moveTo(this->data.moveTo.x,this->data.moveTo.y,this->data.moveTo.time);
            break;
        default:
            break;
        }
        return true;
    }
    return false;
}
BulletDataTime::~BulletDataTime()
{

}
