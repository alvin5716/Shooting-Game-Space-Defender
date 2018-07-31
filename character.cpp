#include "character.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <cmath>
#include <QTimer>
#include "game.h"

Character::Character(QString img, int img_w, int img_h, int show_w, int show_h, int health, int radius, double x, double y, double xv, double yv, double xa, double ya)
    :radius(radius), health(health),
      img_w(img_w), img_h(img_h), show_w(show_w), show_h(show_h), img_timer(0),
      x(x), y(y), xv(xv), yv(yv), xa(xa), ya(ya),
      dead(false), invulnerable(false), whiteized(false), img(img)
{
    setPos(x-show_w/2,y-show_h/2);
    QRect cutFrame(0,0,img_w,img_h);
    QPixmap oriImg(img);
    QPixmap cutImg = oriImg.copy(cutFrame);
    setPixmap(cutImg.scaled(show_w,show_h));
    face_to_left=false;
    canBeMirrored=false;
}
void Character::setPosition(double x, double y) {
    this->x=x;
    this->y=y;
}
void Character::setSpeed(double xv, double yv) {
    this->xv=xv;
    this->yv=yv;
}
void Character::setAcceleration(double xa, double ya) {
    this->xa=xa;
    this->ya=ya;
}
void Character::move() {
    //move object
    setPosition(x+xv,y+yv);
    setSpeed(xv+xa,yv+ya);
}
void Character::img_move() {
    //image move
    if(img_timer%30==0) {
        if(img_timer>=30*4) img_timer=0;
        if(!canBeMirrored) {
            QRect cutFrame(img_w*(img_timer/30),0,img_w,img_h);
            QPixmap oriImg(img);
            QPixmap cutImg = oriImg.copy(cutFrame);
            setPixmap(cutImg.scaled(show_w,show_h));
        } else { // image can be mirrored by direction of speed
            if(face_to_left&&xv>0) face_to_left=false;
            else if((!face_to_left)&&xv<0) face_to_left=true;
            QImage oriImg(img);
            QRect cutFrame((face_to_left)?oriImg.width()-img_w*(img_timer/30+1):img_w*(img_timer/30),0,img_w,img_h);
            QPixmap mirroredImg = QPixmap::fromImage((face_to_left)?oriImg.mirrored(true,false):oriImg);
            QPixmap cutImg = mirroredImg.copy(cutFrame);
            setPixmap(cutImg.scaled(show_w,show_h));
        }
    }
    setPos(x-show_w/2,y-show_h/2);
    ++img_timer;
}
void Character::moveTo(double x, double y, double t) {
    //use physics formula
    setAcceleration((2*(this->x-x))/(t*t),(2*(this->y-y))/(t*t));
    setSpeed(((x>this->x)?1:-1) * sqrt(2*(-this->xa)*(x-this->x)),((y>this->y)?1:-1) * sqrt(2*(-this->ya)*(y-this->y)));
}
Character* Character::testAttackedBy(std::vector<Character*> & attackers) {
    if(x<0-radius || x>Game::FrameWidth+radius || y<0-radius || y>Game::FrameHeight+radius) {
        dead=true;
    }
    for(int i=0;i<(int)attackers.size();++i) {
        if(sqrt(pow(attackers.at(i)->getX() - x,2)+pow(attackers.at(i)->getY() - y,2)) <= attackers.at(i)->getRadius() + radius) {
            attacked();
            return attackers.at(i);
        }
    }
    return NULL;
}
Character* Character::testAttackedBy(Character* attacker) {
    if(x<0-radius || x>Game::FrameWidth+radius || y<0-radius || y>Game::FrameHeight+radius) {
        dead=true;
    }
    if(attacker!=NULL) {
        if(sqrt(pow(attacker->getX() - x,2)+pow(attacker->getY() - y,2)) <= attacker->getRadius() + radius) {
            attacked();
            return attacker;
        }
    }
    return NULL;
}
void Character::attacked() {
    if(health>0 && !invulnerable) {
        health-=1;
        if(health>0) whiteize();
        emit healthChanged(health);
    }
    if(health<=0) {
        dead=true;
        emit deadSignal();
    }
}
void Character::fadein(int time) {
    //effect
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);
    //fade in
    QPropertyAnimation *fadeinAni = new QPropertyAnimation(eff,"opacity");
    fadeinAni->setDuration(time);
    fadeinAni->setStartValue(0);
    fadeinAni->setEndValue(this->opacity());
    fadeinAni->setEasingCurve(QEasingCurve::OutCubic);
    fadeinAni->start(QAbstractAnimation::DeleteWhenStopped);
    //neutralize whiteize effect
    whiteized=true;
    connect(fadeinAni,SIGNAL(finished()),this,SLOT(whiteizedFinish()));
}
void Character::fadeout(int time) {
    //effect
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);
    //fade in
    QPropertyAnimation *fadeoutAni = new QPropertyAnimation(eff,"opacity");
    fadeoutAni->setDuration(time);
    fadeoutAni->setStartValue(this->opacity());
    fadeoutAni->setEndValue(0);
    fadeoutAni->setEasingCurve(QEasingCurve::InCubic);
    fadeoutAni->start(QAbstractAnimation::DeleteWhenStopped);
    //neutralize whiteize effect
    whiteized=true;
    connect(fadeoutAni,SIGNAL(finished()),this,SLOT(whiteizedFinish()));
}
void Character::whiteize(int time) {
    if(whiteized) return;
    whiteized=true;
    //effect
    QGraphicsColorizeEffect *eff = new QGraphicsColorizeEffect(this);
    this->setGraphicsEffect(eff);
    //whiteize
    eff->setColor(QColor(Qt::white));
    QPropertyAnimation *whiteizeAni = new QPropertyAnimation(eff,"strength");
    whiteizeAni->setDuration(time);
    whiteizeAni->setStartValue(0.3);
    whiteizeAni->setEndValue(0);
    whiteizeAni->setEasingCurve(QEasingCurve::InBack);
    connect(whiteizeAni,SIGNAL(finished()),this,SLOT(whiteizedFinish()));
    whiteizeAni->start(QAbstractAnimation::DeleteWhenStopped);
}
void Character::whiteizedFinish() {
    whiteized=false;
}
void Character::setInvulnerable() {
    invulnerable=true;
}
void Character::setVulnerable() {
    invulnerable=false;
}
void Character::killItself() {
    health=0;
    dead=true;
}
double Character::getX() const {
    return x;
}
double Character::getY() const {
    return y;
}
double Character::getRadius() const {
    return radius;
}
int Character::getHealth() const {
    return health;
}
bool Character::isDead() const {
    return dead;
}
bool Character::isInvulnerable() const {
    return invulnerable;
}
double Character::angleofsincos(double sin, double cos) const {
    return (cos>0)?std::asin(sin):M_PI-std::asin(sin);
}
double Character::angleofvector(double x, double y) const {
    if(x==0) return ((y>0)?M_PI/2:3*M_PI/2); //tan is 0/0 or k/0
    if(y==0) return ((x>0)?0:M_PI);
    if(x<0) return std::atan(y/x)+M_PI; //2,3
    if(y>0) return std::atan(y/x); //1
    return std::atan(y/x)+2*M_PI; //4
}
void Character::setCanBeMirrored(bool canBeMirrored) {
    this->canBeMirrored=canBeMirrored;
}
void Character::setFaceToLeft(bool face_to_left) {
    this->face_to_left=face_to_left;
}
