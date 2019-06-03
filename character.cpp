#include "character.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <cmath>
#include <QTimer>
#include "game.h"

int Character::character_count=0;
ImgPixmap Character::imgPixmap(0);

Character::Character(int pixmap, int img_w, int img_h, int show_w, int show_h, int health, int radius, double x, double y, double xv, double yv, double xa, double ya)
    :radius(radius), health(health),
      img_w(img_w), img_h(img_h), show_w(show_w), show_h(show_h), img_timer(0),
      x(x), y(y), xv(xv), yv(yv), xa(xa), ya(ya),
      dead(false), invulnerable(false), whiteized(false), img(""),
      usePixmap(true), pixmap(pixmap), origin(nullptr)
{
    setPos(x-show_w/2,y-show_h/2);
    QRect cutFrame(0,0,img_w,img_h);
    QPixmap *oriImg = imgPixmap.getPixmap(pixmap);
    setPixmap(oriImg->copy(cutFrame).scaled(show_w,show_h));
    face_to_left=false;
    canBeMirrored=false;
    attackable=true;
    floatable=false;
    show_img_setting=false;
    float_timer=0;
    ++character_count;
    //qDebug() << "character count:" << character_count;
}

Character::Character(const QString &img, int img_w, int img_h, int show_w, int show_h, int health, int radius, double x, double y, double xv, double yv, double xa, double ya)
    :radius(radius), health(health),
      img_w(img_w), img_h(img_h), show_w(show_w), show_h(show_h), img_timer(0),
      x(x), y(y), xv(xv), yv(yv), xa(xa), ya(ya),
      dead(false), invulnerable(false), whiteized(false), img(img),
      usePixmap(false), pixmap(-1), origin(nullptr)
{
    setPos(x-show_w/2,y-show_h/2);
    QRect cutFrame(0,0,img_w,img_h);
    QPixmap oriImg(img);
    QPixmap cutImg = oriImg.copy(cutFrame);
    setPixmap(cutImg.scaled(show_w,show_h));
    face_to_left=false;
    canBeMirrored=false;
    attackable=true;
    floatable=false;
    show_img_setting=false;
    float_timer=0;
    ++character_count;
    //qDebug() << "character count:" << character_count;
}
void Character::moveWith(Character* origin) {
    this->origin = origin;
}
void Character::setPosition(double x, double y) {
    this->x=x;
    this->y=y;
}
void Character::setPosition(QPointF pos) {
    this->x=pos.x();
    this->y=pos.y();
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
    if(origin!=nullptr) setPosition(x+origin->xv,y+origin->yv);
    setPosition(x+xv,y+yv);
    setSpeed(xv+xa,yv+ya);
}
void Character::show_img_force_set() {
    show_img_setting=true;
}
void Character::show_img_set() {
    //image move
    if(img_timer%15==0 || show_img_setting) {
        if(img_timer>=15*4) img_timer=0;
        if(!canBeMirrored) {
            QRect cutFrame(img_w*(img_timer/15),0,img_w,img_h);
            if(usePixmap) {
                QPixmap *oriImg = imgPixmap.getPixmap(pixmap);
                this->setPixmap(oriImg->copy(cutFrame).scaled(show_w,show_h));
            } else {
                this->setPixmap(QPixmap(img).copy(cutFrame).scaled(show_w,show_h));
            }
        } else { // image can be mirrored by direction of speed
            if(face_to_left&&xv>0) face_to_left=false;
            else if((!face_to_left)&&xv<0) face_to_left=true;
            QImage oriImg(img);
            QRect cutFrame((face_to_left)?oriImg.width()-img_w*(img_timer/15+1):img_w*(img_timer/15),0,img_w,img_h);
            QPixmap mirroredImg = QPixmap::fromImage((face_to_left)?oriImg.mirrored(true,false):oriImg);
            QPixmap cutImg = mirroredImg.copy(cutFrame);
            setPixmap(cutImg.scaled(show_w,show_h));
        }
        show_img_setting=false;
    }
    ++img_timer;
}
bool Character::floating() const{
    return floatable && std::abs(yv)<=0.1 && std::abs(xv)<=0.1;
}
void Character::img_move() {
    show_img_set();
    if(floating()) {
        setPos(imgX(),imgY()+float_distance*std::sin((double)float_timer/63*M_PI));
        if(++float_timer>=125) float_timer=0;
    } else if(floatable) {
        setPos(imgX(),imgY()+float_distance*std::sin((double)float_timer/63*M_PI));
        if(float_timer%63==0) float_timer=0;
        else if(float_timer%63<=31) --float_timer;
        else ++float_timer;
    } else {
        setPos(imgX(),imgY());
    }
}
double Character::imgX() const{
    return x-show_w/2;
}
double Character::imgY() const{
    return y-show_h/2;
}
void Character::bounceAtX() {
    this->xv *= -1;
    emit bouncedAtX();
}
void Character::bounceAtY() {
    this->yv *= -1;
    emit bouncedAtY();
}
void Character::setFloatable(bool floatable) {
    this->floatable=floatable;
    float_timer=0;
}
void Character::moveTo(double x, double y, double t) {
    //use physics formula
    setAcceleration((2*(this->x-x))/(t*t),(2*(this->y-y))/(t*t));
    setSpeed(((x>this->x)?1:-1) * sqrt(2*(-this->xa)*(x-this->x)),((y>this->y)?1:-1) * sqrt(2*(-this->ya)*(y-this->y)));
}
void Character::moveTo(QPointF pos, double t) {
    double xp = pos.x(), yp = pos.y();
    this->moveTo(xp,yp,t);
}
bool Character::outOfFrame() {
    return x<0-radius || x>Game::FrameWidth+radius || y<0-radius || y>Game::FrameHeight+radius;
}
void Character::deleteIfOutOfFrame() {
    if(this->outOfFrame()) {
        dead=true;
    }
}
Character* Character::testAttackedBy(std::vector<Character*> & attackers) {
    for(int i=0;i<(int)attackers.size();++i) {
        if(attackers.at(i)->isAttackable() && (sqrt(pow(attackers.at(i)->getX() - x,2)+pow(attackers.at(i)->getY() - y,2)) <= attackers.at(i)->getRadius() + radius)) {
            attacked();
            return attackers.at(i);
        }
    }
    return nullptr;
}
Character* Character::testAttackedBy(Character* attacker) {
    if(attacker!=nullptr) {
        if(attacker->isAttackable() && (sqrt(pow(attacker->getX() - x,2)+pow(attacker->getY() - y,2)) <= attacker->getRadius() + radius)) {
            attacked();
            return attacker;
        }
    }
    return nullptr;
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
double Character::getXV() const {
    return xv;
}
double Character::getYV() const {
    return yv;
}
double Character::getRadius() const {
    return radius;
}
int Character::getHealth() const {
    return health;
}
int Character::getShowW() const {
    return show_w;
}
int Character::getShowH() const {
    return show_h;
}
const QString& Character::getImg() const {
    return img;
}
int Character::getPixmap() const {
    return pixmap;
}
bool Character::isDead() const {
    return dead;
}
bool Character::isInvulnerable() const {
    return invulnerable;
}
bool Character::isAttackable() const {
    return attackable;
}
inline double Character::angleofsincos(double sin, double cos) const {
    return (cos>0)?std::asin(sin):M_PI-std::asin(sin);
}
double Character::angleofvector(double x, double y) const {
    if(x==0) return ((y>0)?M_PI/2:3*M_PI/2); //tan is 0/0 or k/0
    if(y==0) return ((x>0)?0:M_PI);
    if(x<0) return std::atan(y/x)+M_PI; //2,3
    if(y>0) return std::atan(y/x); //1
    return std::atan(y/x)+2*M_PI; //4
}
double Character::distanceTo(QPointF another_pos) {
    double x_dif = another_pos.x() - this->x;
    double y_dif = another_pos.y() - this->y;
    return std::sqrt(x_dif*x_dif + y_dif*y_dif);
}
double Character::distanceTo(Character* another_chara) {
    return distanceTo(QPointF(another_chara->getX(), another_chara->getY()));
}
void Character::setCanBeMirrored(bool canBeMirrored) {
    this->canBeMirrored=canBeMirrored;
}
void Character::setFaceToLeft(bool face_to_left) {
    this->face_to_left=face_to_left;
}
void Character::setImg(const QString &img) {
    this->img=img;
    this->usePixmap = false;
}
void Character::setImg(int pixmap) {
    this->pixmap = pixmap;
    this->usePixmap = true;
}
void Character::setShowSize(int show_w, int show_h) {
    this->show_w=show_w;
    this->show_h=show_h;
}
Character::~Character() {
    --character_count;
    //qDebug() << "character count:" << character_count;
}
