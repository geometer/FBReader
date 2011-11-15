#include <QtScroller>
#include <QtScrollerProperties>

#include "ScrollerManager.h"

//ScrollerManager::ScrollerManager()
//{
//}

void ScrollerManager::setScrollerProperties() {
    QtScrollerProperties sp;
    sp.setScrollMetric(QtScrollerProperties::MousePressEventDelay,  qreal(0.1));
    sp.setScrollMetric(QtScrollerProperties::DragStartDistance,   qreal(2.5/1000) );
    sp.setScrollMetric(QtScrollerProperties::DragVelocitySmoothingFactor, qreal(0.85));
    sp.setScrollMetric(QtScrollerProperties::DecelerationFactor, 0.3);
    sp.setScrollMetric(QtScrollerProperties::AcceleratingFlickMaximumTime,  qreal(0.125));
    sp.setScrollMetric(QtScrollerProperties::SnapTime,  qreal(1));
    sp.setScrollMetric(QtScrollerProperties::FrameRate,   QtScrollerProperties::Fps60);
    QtScrollerProperties::setDefaultScrollerProperties(sp);
}

void ScrollerManager::setScroll(QListView *scrollable) {
    scrollable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    scrollable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    scrollable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QtScroller::grabGesture(scrollable->viewport(), QtScroller::LeftMouseButtonGesture);
}
