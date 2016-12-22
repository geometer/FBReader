/****************************************************************************
**
** FBReader
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
**
*****************************************************************************
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
**
****************************************************************************/

#include "ZLQmlSwipeGestureRecognizer.h"
#include <QtGui/QMouseEvent>
#include <QtCore/qmath.h>
#include <QtCore/QDebug>

ZLQmlSwipeGesture::ZLQmlSwipeGesture(QObject *parent) : QSwipeGesture(parent) {
}

ZLQmlSwipeGestureRecognizer::ZLQmlSwipeGestureRecognizer() {
}

ZLQmlSwipeGestureRecognizer::~ZLQmlSwipeGestureRecognizer() {
}

Qt::GestureType ZLQmlSwipeGestureRecognizer::gestureType() {
	static Qt::GestureType type = registerRecognizer(new ZLQmlSwipeGestureRecognizer);
	return type;
}

enum {
	ZLQmlMiminumDistance = 10
};

bool ZLQmlSwipeGestureRecognizer::isValidMove(qreal dx, qreal dy, qint64 msLeft)
{
	// Check if the moved distance is to small to count as not just a glitch.
	return msLeft < 250
	        && (qAbs(dx) > ZLQmlMiminumDistance || qAbs(dy) > ZLQmlMiminumDistance);
}

qreal ZLQmlSwipeGestureRecognizer::computeAngle(qreal dx, qreal dy)
{
	// Need to convert from screen coordinates direction
	// into classical coordinates direction.
	dy = -dy;
	
	qreal result = atan2(dy, dx) ;
	result = (result * qreal(180)) / M_PI;
	
	// Always return positive angle.
	return result < 0 ? result + qreal(360) : result;
}

QGesture *ZLQmlSwipeGestureRecognizer::create(QObject *target) {
	Q_UNUSED(target);
	return new ZLQmlSwipeGesture;
}

QGestureRecognizer::Result ZLQmlSwipeGestureRecognizer::recognize(QGesture *gesture, QObject *watched, QEvent *event) {
	Q_UNUSED(watched);
	QGestureRecognizer::Result result = QGestureRecognizer::Ignore;
	ZLQmlSwipeGesture *swipe = static_cast<ZLQmlSwipeGesture*>(gesture);
    
	switch(event->type()) {
	case QEvent::MouseButtonPress: {
		QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
		swipe->setHotSpot(mouseEvent->globalPos());
		swipe->myStartPoint = mouseEvent->posF();
		swipe->myTimer.start();
		result = QGestureRecognizer::MayBeGesture;
		qDebug() << "Swipe gesture started" << watched;
	}
	break;
	case QEvent::MouseButtonRelease: {
		QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
		QPointF startPoint = swipe->myStartPoint;
		QPointF endPoint = mouseEvent->posF();
		qint64 msLeft = swipe->myTimer.elapsed();
		
		// process distance and direction
		qreal dx = endPoint.x() - startPoint.x();
		qreal dy = endPoint.y() - startPoint.y();
		
		if (!isValidMove(dx, dy, msLeft)) {
			// Just a click, so no gesture.
			result = QGestureRecognizer::CancelGesture;
			qDebug("Swipe gesture canceled");
		} else {
			// Compute the angle.
			qreal angle = computeAngle(dx, dy);
			swipe->setSwipeAngle(angle);
			result = QGestureRecognizer::FinishGesture;
			qDebug() << "Swipe gesture finished" << watched;
		}
	}
	break;
	default:
		break;
	}
    
	return result;
}

void ZLQmlSwipeGestureRecognizer::reset(QGesture *gesture) {
	Q_UNUSED(gesture);
//	ZLQmlSwipeGesture *swipe = static_cast<ZLQmlSwipeGesture*>(gesture);
//	swipe->myTimer.invalidate();
}
