#include "keycontrolbutton.h"
#include <QDebug>

KeyControlButton* KeyControlButton::selected_one = nullptr;

KeyControlButton::KeyControlButton(QWidget* parent)
    :QPushButton(parent),selected(false),arrow(nullptr),arrow_pos(arrowPosLeft)
{
}
void KeyControlButton::setArrowPos(arrowPos arrow_pos) {
    this->arrow_pos = arrow_pos;
}
void KeyControlButton::keyPressEvent(QKeyEvent *e) {
    switch (e->key()) {
    case Qt::Key_Up:
    case Qt::Key_Left:
        if(this->selected) emit upSelect();
        break;
    case Qt::Key_Down:
    case Qt::Key_Right:
        if(this->selected) emit downSelect();
        break;
    case Qt::Key_Z:
    case Qt::Key_Space:
        if(this->selected) emit clicked();
        break;
    default:
        QPushButton::keyPressEvent(e);
    }
}
void KeyControlButton::selectThis() {
    this->selected = true;
    this->setFocus();
    // arrow
    if(arrow==nullptr) {
        arrow = new QLabel(this->parentWidget());
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
        QPixmap oriImg(":/res/player.png");
        QRect cutFrame(0,0,42,32);
        QPixmap cutImg = oriImg.copy(cutFrame);
        arrow->setPixmap(cutImg.scaled(42,32));
        arrow->setStyleSheet("background-color: transparent;");
        arrow->show();
    }
    // unselect else
    if(selected_one!=nullptr) selected_one->unselectThis();
    selected_one = this;
    qDebug() << selected_one->objectName();
}
void KeyControlButton::unselectThis() {
    this->selected = false;
    if(arrow!=nullptr) {
        delete arrow;
        arrow = nullptr;
    }
}
void KeyControlButton::unselect() {
    if(selected_one!=nullptr) selected_one->unselectThis();
    selected_one=nullptr;
}
KeyControlButton::~KeyControlButton() {
    if(arrow!=nullptr) delete arrow;
}
