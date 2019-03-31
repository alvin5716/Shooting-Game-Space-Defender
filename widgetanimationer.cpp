#include "widgetanimationer.h"

WidgetAnimationer::WidgetAnimationer(QWidget *animatedWidget)
    :animatedWidget(animatedWidget), opacityEffect(animatedWidget->parentWidget()),
    fadeinAnimation(&opacityEffect,"opacity"),
    moveInAnimation(animatedWidget,"geometry"),
    expandDir(ExpandDirectionNone), fadeDir(FadeDirectionLeft),
    length(310), opacity(1.0),
    geometry(animatedWidget->geometry())
{
    opacityEffect.setOpacity(opacity);
    animatedWidget->setGraphicsEffect(&opacityEffect);
    fadeinAnimation.setEasingCurve(QEasingCurve::OutCubic);
    moveInAnimation.setEasingCurve(QEasingCurve::OutCubic);
}
void WidgetAnimationer::setOpacity(double opacity) {
    opacityEffect.setOpacity(opacity);
    this->opacity = opacity;
}
void WidgetAnimationer::setGeometry(QRect geometry) {
    animatedWidget->setGeometry(geometry);
    this->geometry = geometry;
}
void WidgetAnimationer::setExpandDir(ExpandDirection expandDir) {
    this->expandDir = expandDir;
}
void WidgetAnimationer::setFadeDir(FadeDirection fadeDir) {
    this->fadeDir = fadeDir;
}
void WidgetAnimationer::animationStart(int duration) {
    setOpacity(opacity);
    setGeometry(geometry);
    //fade
    fadeinAnimation.setStartValue(0);
    fadeinAnimation.setEndValue(opacity);
    fadeinAnimation.setDuration(duration);
    fadeinAnimation.start();
    //geo
    QRect geo(geometry);
    switch(expandDir) {
    case ExpandDirectionHor:
        geo.moveLeft(geo.x()+geo.width()/2);
        geo.setWidth(10);
        break;
    case ExpandDirectionVer:
        geo.moveTop(geo.y()+geo.height()/2);
        geo.setHeight(10);
        break;
    default:
        break;
    }
    switch(fadeDir) {
        case FadeDirectionLeft: geo.moveLeft(geo.x()-length); break;
        case FadeDirectionRight: geo.moveLeft(geo.x()+length); break;
        case FadeDirectionUp: geo.moveTop(geo.y()-length); break;
        case FadeDirectionDown: geo.moveTop(geo.y()+length); break;
        default: break;
    }
    moveInAnimation.setStartValue(geo);
    moveInAnimation.setEndValue(geometry);
    moveInAnimation.setDuration(duration*4/7);
    moveInAnimation.start();
}
void WidgetAnimationer::animationStart3500() {
    animationStart(3500);
}
void WidgetAnimationer::setLength(int length) {
    this->length = length;
}
