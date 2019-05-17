#include "bullet.h"
#include "bulletdatatime.h"
#include "bulletdatawall.h"
#include "bulletdatacontinuous.h"
#include <cmath>
#include <QDebug>
#include "player.h"

Bullet::Bullet(const QString &img, int radius, double x, double y, double xv, double yv, double xa, double ya)
    :Character(img,50,50,(int)round(radius*2.2),(int)round(radius*2.2),1,radius,x,y,xv,yv,xa,ya)
{
    terminal_v = 0;
    enter_timer = 0;
    this->data_head = nullptr;
    this->setPositionByData = false;
    this->lookForward = false;
    this->rotating = img.startsWith(":/res/bullet/3/");
    if(img == ":/res/bullet/other/yellow_hex.png") {
        double const sc = (double)2/std::sqrt(3);
        this->setScale(sc);
    }
}

void Bullet::setVTerminal(double terminal_v) {
    this->terminal_v = terminal_v;
}

void Bullet::waitUntilInFrame(int max_waitting_time) {
    enter_timer = max_waitting_time;
}

void Bullet::setImg(const QString &img) {
    Character::setImg(img);
    this->rotating = img.startsWith(":/res/bullet/3/");
    if(img == ":/res/bullet/other/yellow_hex.png") {
        double const sc = (double)2/std::sqrt(3);
        this->setScale(sc);
    }
}

void Bullet::move() {
    if(this->data_head!=nullptr && this->data_head->skill()) {
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
        if(!setPositionByData) setPosition(x+xv,y+yv);
        if((sqrt(pow(xv,2)+pow(yv,2))<=terminal_v && sqrt(pow(xv+xa,2)+pow(yv+ya,2))<=terminal_v) || (sqrt(pow(xv,2)+pow(yv,2))>=terminal_v && sqrt(pow(xv+xa,2)+pow(yv+ya,2))>=terminal_v)) setSpeed(xv+xa,yv+ya);
    }
    //enter_timer
    if(enter_timer>0) --enter_timer;
}

bool Bullet::outOfFrame() {
    if(enter_timer!=0) {
        if(!Character::outOfFrame()) enter_timer=0;
        return false;
    }
    return Character::outOfFrame();
}

void Bullet::img_move() {
    Character::img_move();
    if(lookForward) {
        double angle = angleofvector(this->xv,this->yv);
        this->setTransformOriginPoint(show_w/2,show_h/2);
        setRotation(angle/M_PI*180);
    } else if(rotating) {
        this->setTransformOriginPoint(show_w/2,show_h/2);
        setRotation(this->rotation()+360.0/63);
    }
}

void Bullet::setLookForward(bool lookForward) {
    this->lookForward=lookForward;
}

void Bullet::rotateStart() {
    this->rotating=true;
}

void Bullet::addData(BulletData* bullet_data) {
    if(this->data_head==nullptr) {
        this->data_head = bullet_data;
        return;
    }
    BulletData* ptr=this->data_head;
    while(ptr->next()!=nullptr) ptr=ptr->next();
    ptr->setNext(bullet_data);
    bullet_data->setNext(nullptr);
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
Bullet* Bullet::addTimeData(int wait_time, Player* player, double v, double a) {
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
Bullet* Bullet::addWallData(Player* player, double v) {
    BulletData* bullet_data = new BulletDataWall(this,player,v);
    this->addData(bullet_data);
    return this;
}
Bullet* Bullet::addWallData(Player* player, double v, double a) {
    BulletData* bullet_data = new BulletDataWall(this,player,v,a);
    this->addData(bullet_data);
    return this;
}
// bullet continuous data
Bullet* Bullet::rotateAround(Character* rotate_center, double rotate_ac, bool clockwise) {
    BulletData* bullet_data = new BulletDataContinuous(this,rotate_center,rotate_ac,clockwise);
    this->addData(bullet_data);
    return this;
}
Bullet* Bullet::rotateAround(int rotate_xc, int rotate_yc, double rotate_ac, bool clockwise) {
    BulletData* bullet_data = new BulletDataContinuous(this,rotate_xc,rotate_yc,rotate_ac,clockwise);
    this->addData(bullet_data);
    return this;
}
Bullet* Bullet::gravityFrom(Character* gravity_center, double gravity_ac) {
    BulletData* bullet_data = new BulletDataContinuous(this,gravity_center,gravity_ac);
    this->addData(bullet_data);
    return this;
}
Bullet* Bullet::gravityFrom(int gravity_xc, int gravity_yc, double gravity_ac) {
    BulletData* bullet_data = new BulletDataContinuous(this,gravity_xc,gravity_yc,gravity_ac);
    this->addData(bullet_data);
    return this;
}
Bullet* Bullet::moveAsTrigFunction(int T, int r, bool sin_or_cos) {
    this->setPositionByData=true;
    BulletData* bullet_data = new BulletDataContinuous(this,T,r,sin_or_cos);
    this->addData(bullet_data);
    return this;
}
Bullet* Bullet::moveAsPeriodicFunction(int T, int r, double (*periodic_func)(double)) {
    this->setPositionByData=true;
    BulletData* bullet_data = new BulletDataContinuous(this,T,r,periodic_func);
    this->addData(bullet_data);
    return this;
}
void Bullet::clearData() {
    if(this->data_head!=nullptr) {
        BulletData *ptr = this->data_head;
        while(ptr->next()!=nullptr) {
            BulletData *temp = ptr;
            ptr=ptr->next();
            delete temp;
        }
        delete ptr;
        this->data_head=nullptr;
    }
}
// clear all bullet data
Bullet::~Bullet() {
    this->clearData();
}
