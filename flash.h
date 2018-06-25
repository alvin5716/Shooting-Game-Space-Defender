#ifndef FLASH_H
#define FLASH_H

#include <QWidget>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTextBrowser>

class Flash : public QTextBrowser
{
    Q_OBJECT
public:
    explicit Flash(QWidget *parent = nullptr);
public slots:
    void flash();
    void flashover();
private:
    QGraphicsOpacityEffect *eff;
    QPropertyAnimation *fadeinAni;
    QPropertyAnimation *fadeoutAni;
};

#endif // FLASH_H
