#ifndef DIALOGUEWIDGET_H
#define DIALOGUEWIDGET_H

#include <QWidget>
#include "widgetanimationer.h"
#include <QLabel>
#include "game.h"

class Dialogue {
public:
    Dialogue(QString content, QString img, QRect img_frame);
    friend class DialogueWidget;
private:
    QString content, img;
    QRect img_frame;
};

class DialogueWidget: public QWidget
{
    Q_OBJECT
public:
    DialogueWidget(QRect geometry, double opacity=0.8, QWidget* parent=nullptr);
    void start(std::initializer_list<Dialogue> dialogues);
    void showNextDialogue();
    void keyPressEvent(QKeyEvent *event);
signals:
    void end();
    void soundPlay(Game::Sound);
private:
    WidgetAnimationer* dialogueAni;
    QLabel* dialogueImg, *dialogueText;
    std::vector<Dialogue> dialogues;
    int current_dia;
};

#endif // DIALOGUEWIDGET_H