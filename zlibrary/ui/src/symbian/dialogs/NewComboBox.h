#ifndef __NEWCOMBOBOX_H__
#define __NEWCOMBOBOX_H__

#include <QtGui/QListView>
#include <QtGui/QPainter>
#include <QtGui/QComboBox>
#include <QtCore/QDebug>

#include "../menu/DrillDownMenu.h"

class NewComboBox : public QComboBox {
    Q_OBJECT
public:
    NewComboBox(QWidget* parent=0);
};

class ComboBoxListView : public QListView {
    Q_OBJECT
public:
    ComboBoxListView(QComboBox *cmb = 0);

protected:
    void resizeEvent(QResizeEvent *event);
    QStyleOptionViewItem viewOptions() const;
    void paintEvent(QPaintEvent *e);

private:
    QComboBox *combo;
};

class ComboMenuDelegate : public QAbstractItemDelegate
{ Q_OBJECT
public:
    ComboMenuDelegate(QObject *parent, QComboBox *cmb) : QAbstractItemDelegate(parent), mCombo(cmb) {}

protected:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const {
        QStyleOptionMenuItem opt = getStyleOption(option, index);
//#ifndef Q_WS_S60
//        painter->fillRect(option.rect, opt.palette.background());
//#endif
        qDebug() << "ComboMenuDelegate" << mCombo->style();
        mCombo->style()->drawControl(QStyle::CE_MenuItem, &opt, painter, mCombo);
    }
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const {
        return MenuItemParameters::getSmallItemSize();
//        QStyleOptionMenuItem opt = getStyleOption(option, index);
//        return mCombo->style()->sizeFromContents(
//            QStyle::CT_MenuItem, &opt, option.rect.size(), mCombo);
    }

private:
    QStyleOptionMenuItem getStyleOption(const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const;
    QComboBox *mCombo;
};

#endif /* NEWCOMBOBOX_H */
