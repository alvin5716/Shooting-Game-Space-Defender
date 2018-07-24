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
    //move image
    setPos(x-show_w/2,y-show_h/2);
    if(img_timer%30==0) {
        if(img_timer>=30*4) img_timer=0;
        QRect cutFrame(img_w*(img_timer/30),0,img_w,img_h);
        QPixmap oriImg(img);
        QPixmap cutImg = oriImg.copy(cutFrame);
        setPixmap(cutImg.scaled(show_w,show_h));
    }
    ++img_timer;
}
void Character::moveTo(double x, double y, double t) {
    //use physics formula
    setAcceleration((2*(this->x-x))/(t*t),(2*(this->y-y))/(t*t));
    setSpeed(((x>this->x)?1:-1) * sqrt(2*(-this->xa)*(x-this->x)),((y>this->y)?1:-1) * sqrt(2*(-this->ya)*(y-this->y)));
}
void Character::sincostoxy(double &sin, double &cos, double aim_x, double aim_y) const {
    double tan = (y-aim_y) / (x-aim_x);
    if(std::isnan(tan)||std::isinf(tan)) { //tan is 0/0 or k/0
        cos=0;
        sin=((aim_x>x)?1:-1);
    } else {
        cos=((aim_x>x)?1:-1)/ sqrt(tan*tan+1);
        sin=tan*cos;
    }
}
void Character::sincostoxy(double &sin, double &cos, double aim_x, double aim_y, double self_x, double self_y) const {
    double tan = (self_y-aim_y) / (self_x-aim_x);
    if(std::isnan(tan)||std::isinf(tan)) { //tan is 0/0 or k/0
        cos=0;
        sin=((aim_x>self_x)?1:-1);
    } else {
        cos=((aim_x>self_x)?1:-1)/ sqrt(tan*tan+1);
        sin=tan*cos;
    }
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
