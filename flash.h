#ifndef FLASH_H
#define FLASH_H

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTextBrowser>
#include <QGraphicsRectItem>

class Flash : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    explicit Flash(QRect geo);
    void setOpacity(double opacity);
    void setFlashTime(int fadeinTime=100, int fadeoutTime=100);
public slots:
    void flash(bool deleteWhenStopped=false);
    void flashover();
    void killItself();
    void hide();
private:
    QGraphicsOpacityEffect *eff;
    QPropertyAnimation *fadeinAni;
    QPropertyAnimation *fadeoutAni;
};

#endif // FLASH_H
