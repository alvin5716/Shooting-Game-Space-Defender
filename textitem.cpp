#include "textitem.h"

TextItem::TextItem(const QString &text, QGraphicsItem *parent)
    :QGraphicsTextItem(text, parent)
{
}

void TextItem::showSlot() {
    this->show();
}

void TextItem::hideSlot() {
    this->hide();
}

void TextItem::setTextSlot(const QString &text) {
    this->setPlainText(text);
}
