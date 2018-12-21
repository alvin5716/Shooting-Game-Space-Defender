#ifndef ANIMATIONWIDGET_H
#define ANIMATIONWIDGET_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QGraphicsEffect>
#include <QDebug>

class WidgetAnimationer: public QObject
{
    Q_OBJECT
public:
    enum FadeDirection{
        FadeDirectionUp,
        FadeDirectionDown,
        FadeDirectionRight,
        FadeDirectionLeft
    };
    WidgetAnimationer() = delete;
    explicit WidgetAnimationer(QWidget *animatedWidget)
        :animatedWidget(animatedWidget), opacityEffect(animatedWidget->parentWidget()),
        fadeinAnimation(&opacityEffect,"opacity"),
        moveInAnimation(animatedWidget,"geometry"),
        fadeDir(FadeDirectionLeft), opacity(1.0), geometry(animatedWidget->geometry())
    {
        opacityEffect.setOpacity(opacity);
        animatedWidget->setGraphicsEffect(&opacityEffect);
        fadeinAnimation.setEasingCurve(QEasingCurve::OutCubic);
        moveInAnimation.setEasingCurve(QEasingCurve::OutCubic);
    }
    void setOpacity(double opacity) {
        opacityEffect.setOpacity(opacity);
        this->opacity = opacity;
    }
    void setGeometry(QRect geometry) {
        animatedWidget->setGeometry(geometry);
        this->geometry = geometry;
    }
    void setFadeDir(FadeDirection fadeDir=FadeDirectionLeft) {
        this->fadeDir = fadeDir;
    }
public slots:
    void animationStart(int duration=1500) {
        setOpacity(opacity);
        setGeometry(geometry);
        //fade
        fadeinAnimation.setStartValue(0);
        fadeinAnimation.setEndValue(opacity);
        fadeinAnimation.setDuration(duration);
        fadeinAnimation.start();
        //geo
        QRect geo(geometry);
        switch(fadeDir) {
            case FadeDirectionLeft: geo.moveLeft(geo.x()-310); break;
            case FadeDirectionRight: geo.moveLeft(geo.x()+310); break;
            case FadeDirectionUp: geo.moveTop(geo.y()-310); break;
            case FadeDirectionDown: geo.moveTop(geo.y()+310); break;
        }
        moveInAnimation.setStartValue(geo);
        moveInAnimation.setEndValue(geometry);
        moveInAnimation.setDuration(duration*4/7);
        moveInAnimation.start();
    }
    void animationStart3500() {
        animationStart(3500);
    }
private:
    QWidget* animatedWidget;
    QGraphicsOpacityEffect opacityEffect;
    QPropertyAnimation fadeinAnimation, moveInAnimation;
    FadeDirection fadeDir;
    double opacity;
    QRect geometry;
};

#endif // ANIMATIONWIDGET_H
