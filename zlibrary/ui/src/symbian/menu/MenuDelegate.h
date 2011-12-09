#ifndef __MENU_DELEGATE_H__
#define __MENU_DELEGATE_H__

#include <QtGui/QStyledItemDelegate>
#include <QtGui/QPainter>
#include <QtGui/QStyleOption>
#include <QtGui/QStyle>

class SeparatorDelegate : public QStyledItemDelegate {

public:
    explicit SeparatorDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:
    void drawSeparator(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif /* __MENU_DELEGATE_H__ */
