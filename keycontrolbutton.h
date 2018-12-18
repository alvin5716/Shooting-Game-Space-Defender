#ifndef KEYCONTROLBUTTON_H
#define KEYCONTROLBUTTON_H

#include <QPushButton>
#include <QLabel>
#include <QKeyEvent>


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
    void unselectThis();
signals:
    void upSelect();
    void downSelect();
private:
    static KeyControlButton* selected_one;
    bool selected;
    QLabel* arrow;
    arrowPos arrow_pos;
};

#endif // KEYCONTROLBUTTON_H
