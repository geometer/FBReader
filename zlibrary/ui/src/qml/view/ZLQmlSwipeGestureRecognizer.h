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

#ifndef ZLQMLSWIPEGESTURERECOGNIZER_H
#define ZLQMLSWIPEGESTURERECOGNIZER_H

#include <QtGui/QGestureRecognizer>
#include <QtGui/QSwipeGesture>
#include <QtCore/QElapsedTimer>

class ZLQmlSwipeGesture : public QSwipeGesture
{
	Q_OBJECT
public:
	ZLQmlSwipeGesture(QObject *parent = 0);
	
	QPointF myStartPoint;
	QElapsedTimer myTimer;
};

class ZLQmlSwipeGestureRecognizer : public QGestureRecognizer
{
public:
    ZLQmlSwipeGestureRecognizer();
	virtual ~ZLQmlSwipeGestureRecognizer();
	
	static Qt::GestureType gestureType();
	
	bool isValidMove(qreal dx, qreal dy, qint64 msLeft);
    qreal computeAngle(qreal dx, qreal dy);
	
    virtual QGesture *create(QObject *target);
    virtual Result recognize(QGesture *gesture, QObject *watched, QEvent *event);
    virtual void reset(QGesture *gesture);
	
private:
	
};

#endif // ZLQMLSWIPEGESTURERECOGNIZER_H
