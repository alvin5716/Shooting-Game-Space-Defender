#include "itemanimationer.h"

ItemAnimationer::ItemAnimationer(QGraphicsObject *animatedItem)
    :animatedItem(animatedItem), opacityEffect(nullptr),
    fadeinAnimation(&opacityEffect,"opacity"),
    moveInAnimation(animatedItem,"pos"),
    fadeDir(FadeDirectionLeft),
    length(310), opacity(1.0),
    pos(animatedItem->pos())
{
    opacityEffect.setOpacity(opacity);
    animatedItem->setGraphicsEffect(&opacityEffect);
    fadeinAnimation.setEasingCurve(QEasingCurve::OutCubic);
    moveInAnimation.setEasingCurve(QEasingCurve::OutCubic);
}
void ItemAnimationer::setOpacity(double opacity) {
    opacityEffect.setOpacity(opacity);
    this->opacity = opacity;
}
void ItemAnimationer::setPos(QPointF pos) {
    animatedItem->setPos(pos);
    this->pos = pos;
}
void ItemAnimationer::setFadeDir(FadeDirection fadeDir) {
    this->fadeDir = fadeDir;
}
void ItemAnimationer::animationStart(int duration) {
    setOpacity(opacity);
    setPos(pos);
    //fade
    fadeinAnimation.setStartValue(0);
    fadeinAnimation.setEndValue(opacity);
    fadeinAnimation.setDuration(duration);
    fadeinAnimation.start();
    //p
    QPointF p(pos);
    switch(fadeDir) {
        case FadeDirectionLeft: p.setX(p.x()-length); break;
        case FadeDirectionRight: p.setX(p.x()+length); break;
        case FadeDirectionUp: p.setY(p.y()-length); break;
        case FadeDirectionDown: p.setY(p.y()+length); break;
        default: break;
    }
    moveInAnimation.setStartValue(p);
    moveInAnimation.setEndValue(pos);
    moveInAnimation.setDuration(duration*4/7);
    moveInAnimation.start();
}
void ItemAnimationer::animationStart3500() {
    animationStart(3500);
}
void ItemAnimationer::setLength(int length) {
    this->length = length;
}
