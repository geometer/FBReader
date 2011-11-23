#include <QtScroller>
#include <QtScrollerProperties>
#include <QtCore/QDebug>

#include "ScrollerManager.h"

//ScrollerManager::ScrollerManager()
//{
//}

void ScrollerManager::setScrollerProperties() {
    QtScrollerProperties sp;
    sp.setScrollMetric(QtScrollerProperties::MousePressEventDelay,  qreal(0.5));
    sp.setScrollMetric(QtScrollerProperties::DragStartDistance,   qreal(2.5/1000) );
    sp.setScrollMetric(QtScrollerProperties::MaximumClickThroughVelocity,   qreal(0) );
//    sp.setScrollMetric(QtScrollerProperties::DragVelocitySmoothingFactor, qreal(0.85));
//    sp.setScrollMetric(QtScrollerProperties::DecelerationFactor, 0.3);
//    sp.setScrollMetric(QtScrollerProperties::AcceleratingFlickMaximumTime,  qreal(0.125));
//    sp.setScrollMetric(QtScrollerProperties::SnapTime,  qreal(1));
//    sp.setScrollMetric(QtScrollerProperties::FrameRate,   QtScrollerProperties::Standard);
    QtScrollerProperties::setDefaultScrollerProperties(sp);
}


void ScrollerManager::setScroll(QAbstractItemView* scrollable) {
    qDebug() << Q_FUNC_INFO;
    scrollable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    scrollable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setScroll(static_cast<QAbstractScrollArea*>(scrollable));
}

void ScrollerManager::setScroll(QAbstractScrollArea *scrollable) {
    qDebug() << Q_FUNC_INFO;
    scrollable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QtScroller::grabGesture(scrollable->viewport(), QtScroller::LeftMouseButtonGesture);
}
