#include "flash.h"
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QDebug>
#include <QPalette>
#include "game.h"

Flash::Flash(QWidget *parent) : QTextBrowser(parent)
{
    //color
    this->setStyleSheet("background-color: white;");
    //init
    setGeometry(51,30,Game::FrameWidth,Game::FrameHeight);
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
void Flash::flash() {
    show();
    fadeinAni->start();
}
void Flash::flashover() {
    fadeoutAni->start(QAbstractAnimation::KeepWhenStopped);
}
