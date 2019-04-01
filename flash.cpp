#include "flash.h"
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QDebug>
#include <QPalette>
#include <QGraphicsView>
#include "game.h"

Flash::Flash(QRect geo, QWidget *parent) : QFrame(parent)
{
    //color
    this->setStyleSheet("background-color: white;");
    //init
    this->setGeometry(geo);
    hide();
    //effect
    eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);
    //fade in
    fadeinAni = new QPropertyAnimation(eff,"opacity");
    fadeinAni->setDuration(100);
    fadeinAni->setStartValue(0);
    fadeinAni->setEndValue(1);
    fadeinAni->setEasingCurve(QEasingCurve::OutCubic);
    //fade out
    fadeoutAni = new QPropertyAnimation(eff,"opacity");
    fadeoutAni->setDuration(100);
    fadeoutAni->setStartValue(1);
    fadeoutAni->setEndValue(0);
    fadeoutAni->setEasingCurve(QEasingCurve::InCubic);
    //fadeout after fadein
    connect(fadeinAni,SIGNAL(finished()),this,SLOT(flashover()));
    //hide after fadeout
    connect(fadeoutAni,SIGNAL(finished()),this,SLOT(hide()));
}
void Flash::flash(bool deleteWhenStopped) {
    if(deleteWhenStopped) connect(fadeoutAni,SIGNAL(finished()),this,SLOT(killItself()));
    show();
    fadeinAni->start();
}
void Flash::flashover() {
    fadeoutAni->start();
}
void Flash::setOpacity(double opacity) {
    fadeinAni->setEndValue(opacity);
    fadeoutAni->setStartValue(opacity);
}
void Flash::setFlashTime(int fadeinTime, int fadeoutTime) {
    fadeoutAni->setDuration(fadeinTime);
    fadeoutAni->setDuration(fadeoutTime);
}
void Flash::killItself() {
    delete fadeinAni;
    delete fadeoutAni;
    delete eff;
    delete this;
}
