#ifndef TEXTITEM_H
#define TEXTITEM_H

#include <QGraphicsTextItem>
#include <QPropertyAnimation>

class TextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    TextItem(const QString& text, QGraphicsItem* parent = nullptr);
public slots:
    void showSlot();
    void hideSlot();
    void setTextSlot(const QString &text);
};

#endif // TEXTITEM_H
