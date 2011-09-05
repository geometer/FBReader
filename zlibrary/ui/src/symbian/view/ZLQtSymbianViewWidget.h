#ifndef ZLQTSYMBIANVIEWWIDGET_H
#define ZLQTSYMBIANVIEWWIDGET_H


#include "ZLQtViewWidget.h"


class ZLQtSymbianViewWidget : public ZLQtViewWidget {

public:
    ZLQtSymbianViewWidget(QWidget *parent, ZLApplication *application);
private:
    QScrollBar *addScrollBar(QGridLayout *layout, Qt::Orientation orientation, int x, int y);

};

#endif // ZLQTSYMBIANVIEWWIDGET_H
