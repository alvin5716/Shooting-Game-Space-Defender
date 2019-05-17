#ifndef ITEMWIDGET_H
#define ITEMWIDGET_H

#include <QGraphicsItem>
#include <QPropertyAnimation>
#include <QGraphicsEffect>
#include <QDebug>

class ItemAnimationer: public QObject
{
    Q_OBJECT
public:
    ItemAnimationer() = delete;
    explicit ItemAnimationer(QGraphicsObject *animatedItem);
    enum FadeDirection{
        FadeDirectionUp,
        FadeDirectionDown,
        FadeDirectionRight,
        FadeDirectionLeft,
        FadeDirectionNone
    };
    void setOpacity(double opacity);
    void setPos(QPointF pos);
    void setFadeDir(FadeDirection fadeDir=FadeDirectionLeft);
public slots:
    void animationStart(int duration=1500);
    void animationStart3500();
    void setLength(int length=310);
private:
    QGraphicsObject* animatedItem;
    QGraphicsOpacityEffect opacityEffect;
    QPropertyAnimation fadeinAnimation, moveInAnimation;
    FadeDirection fadeDir;
    int length;
    double opacity;
    QPointF pos;
};

#endif // ITEMWIDGET_H
