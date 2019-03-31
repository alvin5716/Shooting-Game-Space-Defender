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
    enum ExpandDirection{
        ExpandDirectionHor,
        ExpandDirectionVer,
        ExpandDirectionNone
    };
    enum FadeDirection{
        FadeDirectionUp,
        FadeDirectionDown,
        FadeDirectionRight,
        FadeDirectionLeft,
        FadeDirectionNone
    };
    WidgetAnimationer() = delete;
    explicit WidgetAnimationer(QWidget *animatedWidget);
    void setOpacity(double opacity);
    void setGeometry(QRect geometry);
    void setExpandDir(ExpandDirection expandDir=ExpandDirectionNone);
    void setFadeDir(FadeDirection fadeDir=FadeDirectionLeft);
public slots:
    void animationStart(int duration=1500);
    void animationStart3500();
    void setLength(int length=310);
private:
    QWidget* animatedWidget;
    QGraphicsOpacityEffect opacityEffect;
    QPropertyAnimation fadeinAnimation, moveInAnimation;
    ExpandDirection expandDir;
    FadeDirection fadeDir;
    int length;
    double opacity;
    QRect geometry;
};

#endif // ANIMATIONWIDGET_H
