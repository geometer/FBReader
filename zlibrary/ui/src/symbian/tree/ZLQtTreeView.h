#ifndef __ZLQTTREEVIEW_H__
#define __ZLQTTREEVIEW_H__

#include <QtGui/QApplication>
#include <QtGui/QListView>
#include <QtGui/QStyledItemDelegate>
#include <QtGui/QPainter>
#include <QtGui/QStyleOption>
#include <QtGui/QStyle>

#include "../menu/DrillDownMenu.h"

class ZLQtTreeView : public QListView {
    Q_OBJECT
public:
    explicit ZLQtTreeView(QWidget *parent = 0);

signals:

public slots:

};

class SubtitleDelegate : public QStyledItemDelegate {
public:
    explicit SubtitleDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    void drawTitle(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void drawSubtitle(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void drawIcon(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void drawHighlight(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    static QRect getCenteredRectangle(QRect zoneRect, QSize imageSize);
    static int getTextLeftMargin(const QStyleOptionViewItem &option, const QModelIndex &index);
    static int getLeftMargin();

};



#endif // ZLQTTREEVIEW_H
