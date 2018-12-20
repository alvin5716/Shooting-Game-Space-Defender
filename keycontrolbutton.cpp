#include "keycontrolbutton.h"
#include "game.h"
#include <QDebug>

KeyControlButton* KeyControlButton::selected_one = nullptr;
FlashLabel* KeyControlButton::arrow = nullptr;

KeyControlButton::KeyControlButton(QWidget* parent)
    :QPushButton(parent),arrow_pos(arrowPosLeft)
{
    connect(this,&KeyControlButton::clicked,[this](){
        emit soundPlay(Game::SoundFire);
    });
}
void KeyControlButton::setArrowPos(arrowPos arrow_pos) {
    this->arrow_pos = arrow_pos;
}
void KeyControlButton::keyPressEvent(QKeyEvent *e) {
    switch (e->key()) {
    case Qt::Key_Up:
    case Qt::Key_Left:
        if(this->isSelected()) {
            emit upSelect();
            emit soundPlay(Game::SoundSnare);
        }
        break;
    case Qt::Key_Down:
    case Qt::Key_Right:
        if(this->isSelected()) {
            emit downSelect();
            emit soundPlay(Game::SoundSnare);
        }
        break;
    case Qt::Key_Z:
    case Qt::Key_Space:
        if(this->isSelected()) emit clicked();
        break;
    default:
        QPushButton::keyPressEvent(e);
    }
}
void KeyControlButton::selectThis() {
    this->setFocus();
    // arrow
    if(arrow==nullptr) {
        arrow = new FlashLabel(this->parentWidget());
        QPixmap oriImg(":/res/player.png");
        QRect cutFrame(0,0,42,32);
        QPixmap cutImg = oriImg.copy(cutFrame);
        arrow->setPixmap(cutImg.scaled(42,32));
        arrow->setStyleSheet("background-color: transparent;");
        arrow->show();
    }
    arrow->setParent(this->parentWidget());
    const int width = 42, height = 32;
    switch (arrow_pos) {
    case arrowPosLeft:
        arrow->setGeometry(QRect(this->x()-30-width/2,this->y()+this->height()/2-height/2,width,height));
        break;
    case arrowPosRight:
        arrow->setGeometry(QRect(this->x()+this->width()+30-width/2,this->y()+this->height()/2-height/2,42,32));
        break;
    case arrowPosUp:
        arrow->setGeometry(QRect(this->x()+this->width()/2-width/2,this->y()-30-height/2,42,32));
        break;
    case arrowPosDown:
        arrow->setGeometry(QRect(this->x()+this->width()/2-width/2,this->y()+this->height()+30-height/2,42,32));
        break;
    }
    selected_one = this;
    arrow->showAndResetTimer();
    qDebug() << selected_one->objectName();
}
void KeyControlButton::unselect() {
    if(selected_one!=nullptr) {
        delete arrow;
        arrow = nullptr;
    }
    selected_one=nullptr;
}
bool KeyControlButton::isSelected() {
    return this == selected_one;
}
KeyControlButton::~KeyControlButton() {
    if(isSelected()) delete arrow;
}
