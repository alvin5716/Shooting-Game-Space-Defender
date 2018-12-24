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
        fadeDir(FadeDirectionLeft), opacity(1.0), geometry(animatedWidget->geometry()),
        length(310)
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
            case FadeDirectionLeft: geo.moveLeft(geo.x()-length); break;
            case FadeDirectionRight: geo.moveLeft(geo.x()+length); break;
            case FadeDirectionUp: geo.moveTop(geo.y()-length); break;
            case FadeDirectionDown: geo.moveTop(geo.y()+length); break;
        }
        moveInAnimation.setStartValue(geo);
        moveInAnimation.setEndValue(geometry);
        moveInAnimation.setDuration(duration*4/7);
        moveInAnimation.start();
    }
    void animationStart3500() {
        animationStart(3500);
    }
    void setLength(int length=310) {
        this->length = length;
    }
private:
    QWidget* animatedWidget;
    QGraphicsOpacityEffect opacityEffect;
    QPropertyAnimation fadeinAnimation, moveInAnimation;
    FadeDirection fadeDir;
    double opacity;
    QRect geometry;
    int length;
};

#endif // ANIMATIONWIDGET_H
