#include "flashlabel.h"

FlashLabel::FlashLabel(QWidget* parent, int interval)
    :QLabel(parent), timer()
{
    timer.start(interval);
    connect(&timer,SIGNAL(timeout()),this,SLOT(hideOrShow()));
}

void FlashLabel::hideOrShow() {
    if(this->isHidden()) this->show();
    else this->hide();
}

void FlashLabel::showAndResetTimer() {
    if(this->isHidden()) this->show();
    timer.start(timer.interval());
}
