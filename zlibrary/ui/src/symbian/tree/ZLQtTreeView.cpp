#include "ZLQtTreeModel.h"
#include "ZLQtTreeView.h"

ZLQtTreeView::ZLQtTreeView(QWidget *parent) :  QListView(parent) {
}


SubtitleDelegate::SubtitleDelegate(QObject *parent) : QStyledItemDelegate(parent) {

}

void SubtitleDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    //qDebug() << Q_FUNC_INFO << index.data() << index.data(ZLQtTreeModel::SubTitleRole);
    QStyledItemDelegate::paint(painter,option,index);
    QString subtitle = index.data(ZLQtTreeModel::SubTitleRole).toString();
    if (!subtitle.isEmpty()) {
        painter->save();
        painter->setClipRect(option.rect);
        painter->setPen( option.state & QStyle::State_HasFocus ?
                         option.palette.highlightedText().color() :
                         option.palette.text().color());
        const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin, &option) + 2;
        const int iconMargin = MenuItemParameters::getImageSize().width() +
                QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing, &option);
        QRect textRect = option.rect.adjusted(textMargin+iconMargin, 0, -(textMargin+iconMargin), 0);
        painter->setFont(MenuItemParameters::getSubtitleFont());
        QString elidedSubtitle = painter->fontMetrics().elidedText(subtitle,Qt::ElideRight, textRect.width());
        painter->drawText(textRect, Qt::AlignBottom,  elidedSubtitle);
        painter->restore();
    }

}
QSize SubtitleDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    //qDebug() << Q_FUNC_INFO << index;
    return QStyledItemDelegate::sizeHint(option,index);
}
