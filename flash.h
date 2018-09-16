#ifndef FLASH_H
#define FLASH_H

#include <QWidget>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTextBrowser>

class Flash : public QFrame
{
    Q_OBJECT
public:
    explicit Flash(QWidget *parent = nullptr);
    void setOpacity(double opacity);
    void setFlashTime(int fadeinTime=100, int fadeoutTime=100);
public slots:
    void flash(bool deleteWhenStopped=false);
    void flashover();
    void killItself();
private:
    QGraphicsOpacityEffect *eff;
    QPropertyAnimation *fadeinAni;
    QPropertyAnimation *fadeoutAni;
};

#endif // FLASH_H
