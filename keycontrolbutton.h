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
    enum arrowPos{
        arrowPosUp,
        arrowPosDown,
        arrowPosLeft,
        arrowPosRight,
    };
    void keyPressEvent(QKeyEvent *e);
    void setArrowPos(arrowPos arrow_pos);
    static void unselect();
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
    arrowPos arrow_pos;
};

#endif // KEYCONTROLBUTTON_H
