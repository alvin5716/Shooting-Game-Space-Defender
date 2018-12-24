#include "dialoguewidget.h"
#include <QKeyEvent>

Dialogue::Dialogue(QString content, QString img, QRect img_frame)
    :content(content), img(img), img_frame(img_frame)
{

}

DialogueWidget::DialogueWidget(QRect geometry, double opacity, QWidget* parent)
    :QWidget(parent),
      dialogueAni(new WidgetAnimationer(this)),
      dialogueImg(new QLabel(this)),
      dialogueText(new QLabel(this)),
      current_dia(-1)
{
    this->setGeometry(geometry);
    dialogueText->setGeometry(220,30,511,180);
    dialogueText->setStyleSheet("QLabel {"
                               "    border-style: solid;"
                               "    border-color: white;"
                               "    border-width: 5px 5px 5px 0px;"
                               "    color: white;"
                               "    border-radius: 5px;"
                               "}");
    dialogueText->setFont(QFont("微軟正黑體",18,QFont::Bold));
    dialogueText->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    dialogueImg->setGeometry(0,0,220,210);
    dialogueImg->setStyleSheet("QLabel {"
                               "    border-style: solid;"
                               "    border-color: white;"
                               "    border-width: 5px;"
                               "    border-radius: 5px;"
                               "}");
    dialogueAni->setGeometry(geometry);
    dialogueAni->setOpacity(opacity);
    dialogueAni->setFadeDir(WidgetAnimationer::FadeDirectionUp);
    dialogueAni->setLength(120);
}

void DialogueWidget::start(std::initializer_list<Dialogue> dialogues) {
    dialogueAni->animationStart();
    this->dialogues.clear();
    this->dialogues.shrink_to_fit();
    for(Dialogue dialogue: dialogues) {
        this->dialogues.push_back(dialogue);
    }
    current_dia = -1;
    showNextDialogue();
}
void DialogueWidget::showNextDialogue() {
    if(++this->current_dia>=dialogues.size()) {
        emit end();
        return;
    }
    Dialogue dialogue(dialogues.at(this->current_dia));
    QRect cutFrame(dialogue.img_frame);
    QPixmap oriImg(dialogue.img);
    QPixmap cutImg = oriImg.copy(cutFrame);
    dialogueImg->setPixmap(cutImg.scaled(dialogueImg->width(),dialogueImg->height()));
    dialogueText->setText(dialogue.content);
}
void DialogueWidget::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Z:
        showNextDialogue();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}
