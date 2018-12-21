#ifndef KEYCONTROLBUTTON_H
#define KEYCONTROLBUTTON_H

#include <QPushButton>
#include <QKeyEvent>
#include "game.h"
#include "flashlabel.h"

class KeyControlButton : public QPushButton
{
    Q_OBJECT
public:
    KeyControlButton(QWidget* parent=nullptr);
    enum ArrowPos {
        ArrowPosUp,
        ArrowPosDown,
        ArrowPosLeft,
        ArrowPosRight,
    };
    void keyPressEvent(QKeyEvent *e);
    void setArrowPos(ArrowPos arrow_pos);
    static void unselect();
    static bool setFocusOnSelectedOne();
    void disableClick(int duration);
    ~KeyControlButton();
public slots:
    void selectThis();
signals:
    void upSelect();
    void downSelect();
    void soundPlay(Game::Sound);
private:
    static KeyControlButton* selected_one;
    static FlashLabel* arrow;
    bool isSelected();
    ArrowPos arrow_pos;
    bool clickable;
    QTimer* disableClickTimer;
};

#endif // KEYCONTROLBUTTON_H
