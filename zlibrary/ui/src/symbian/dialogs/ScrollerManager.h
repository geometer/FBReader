#ifndef __SCROLLERMANAGER_H__
#define __SCROLLERMANAGER_H__

#include <QtGui/QListWidget>

class ScrollerManager {
public:
    //ScrollerManager();
public:
    static void setScrollerProperties();
    static void setScroll(QAbstractItemView* widget);


};

#endif /* SCROLLERMANAGER_H */
