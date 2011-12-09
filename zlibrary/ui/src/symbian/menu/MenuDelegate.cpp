#include "MenuDelegate.h"

SeparatorDelegate::SeparatorDelegate(QObject *parent) : QStyledItemDelegate(parent) {

}

void SeparatorDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter,option,index);
    drawSeparator(painter,option,index);
}

void SeparatorDelegate::drawSeparator(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (index.row() == 0) {
        return;
    }
    painter->save();
    painter->setClipRect(option.rect);
    static const int COLOR_DIFF = 70; //approximately 30% of 255
    QColor separator = option.palette.color(QPalette::Background);
    int lightness = separator.lightness();
    if ((lightness + COLOR_DIFF) <= 255) {
        lightness += COLOR_DIFF;
    } else {
        lightness -= COLOR_DIFF;
    }
    separator.setHsl(separator.hslHue(), separator.hslSaturation(), lightness);
    painter->setPen(separator);
    painter->drawLine(option.rect.topLeft(), option.rect.topRight());
    painter->restore();
}
