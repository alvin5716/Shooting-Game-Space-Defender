#ifndef FLASHLABEL_H
#define FLASHLABEL_H

#include <QLabel>
#include <QTimer>
#include <QGraphicsOpacityEffect>

class FlashLabel : public QLabel
{
    Q_OBJECT
public:
    FlashLabel(QWidget *parent=nullptr, int interval=800);
public slots:
    void hideOrShow();
    void showAndResetTimer();
private:
    QTimer timer;
};

#endif // FLASHLABEL_H
