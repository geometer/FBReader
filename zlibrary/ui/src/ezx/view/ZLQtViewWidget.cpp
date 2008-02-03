/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <algorithm>

#include <qpainter.h>
#include <qpixmap.h>
#include <qframe.h>
#include <qimage.h>
#include <zapplication.h>

#include "ZLQtViewWidget.h"
#include "ZLQtPaintContext.h"
#include "../application/ZLQtApplicationWindow.h"
#include "../dialogs/ZLQtDialogManager.h"

ZLQtViewWidget::ZLQtViewWidgetInternal::ZLQtViewWidgetInternal(QWidget *parent, ZLQtViewWidget &holder) : QWidget(parent), myHolder(holder), myTimerId(0), myWasLeaved(false) {
	setBackgroundMode(NoBackground);
}

ZLQtViewWidget::ZLQtViewWidget(QWidget *parent, ZLQtApplicationWindow &applicationWindow) : ZLViewWidget((ZLViewWidget::Angle)applicationWindow.application().AngleStateOption.value()), myApplicationWindow(applicationWindow) {
	myQWidget = new ZLQtViewWidgetInternal(parent, *this);
}

void ZLQtViewWidget::trackStylus(bool track) {
	myQWidget->setMouseTracking(track);
}

void ZLQtViewWidget::ZLQtViewWidgetInternal::paintEvent(QPaintEvent*) {
	const int w = width();
	const int h = height() + myHolder.myApplicationWindow.verticalAdjustment();
	switch (myHolder.rotation()) {
		default:
			((ZLQtPaintContext&)myHolder.view()->context()).setSize(w, h);
			break;
		case DEGREES90:
		case DEGREES270:
			((ZLQtPaintContext&)myHolder.view()->context()).setSize(h, w);
			break;
	}
	myHolder.view()->paint();

	int angle = 0;
	switch (myHolder.rotation()) {
		default:
			break;
		case DEGREES90:
			angle = 270;
			break;
		case DEGREES180:
			angle = 180;
			break;
		case DEGREES270:
			angle = 90;
			break;
	}
	const QPixmap &pixmap = ((ZLQtPaintContext&)myHolder.view()->context()).pixmap();
	QPainter painter(this);
	if (angle == 0) {
		painter.drawPixmap(0, 0, pixmap);
	} else {
		QWMatrix matrix;
		matrix.rotate(angle);
		painter.drawPixmap(0, 0, pixmap.xForm(matrix));
	}
}

void ZLQtViewWidget::ZLQtViewWidgetInternal::mousePressEvent(QMouseEvent *event) {
	myWasLeaved = false;
	if (myTimerId > 0) {
		killTimer(myTimerId);
		myTimerId = 0;
	}

	myCurrentX = x(event);
	myCurrentY = y(event);

	myHolder.view()->onStylusMove(myCurrentX, myCurrentY);
	myHolder.view()->onStylusPress(myCurrentX, myCurrentY);

	if (!myWasLeaved) {
		myTimerId = startTimer(50);
	}
}

void ZLQtViewWidget::ZLQtViewWidgetInternal::timerEvent(QTimerEvent*) {
	myHolder.view()->onStylusPress(myCurrentX, myCurrentY);
}

void ZLQtViewWidget::ZLQtViewWidgetInternal::leaveEvent(QEvent*) {
	if (myTimerId > 0) {
		killTimer(myTimerId);
		myTimerId = 0;
	}
	myWasLeaved = true;
}

void ZLQtViewWidget::ZLQtViewWidgetInternal::mouseReleaseEvent(QMouseEvent *event) {
	if (myTimerId > 0) {
		killTimer(myTimerId);
		myTimerId = 0;
	}
	myHolder.view()->onStylusRelease(x(event), y(event));
}

void ZLQtViewWidget::ZLQtViewWidgetInternal::mouseMoveEvent(QMouseEvent *event) {
	switch (event->state() & MouseButtonMask) {
		case LeftButton:
			myHolder.view()->onStylusMovePressed(x(event), y(event));
			break;
		case NoButton:
			myHolder.view()->onStylusMove(x(event), y(event));
			break;
		default:
			break;
	}

	myCurrentX = x(event);
	myCurrentY = y(event);
}

int ZLQtViewWidget::ZLQtViewWidgetInternal::x(const QMouseEvent *event) const {
	const int maxX = width() - 1;
	const int maxY = height() + myHolder.myApplicationWindow.verticalAdjustment() - 1;
	switch (myHolder.rotation()) {
		default:
			return std::min(std::max(event->x(), 0), maxX);
		case DEGREES90:
			return maxY - std::min(std::max(event->y(), 0), maxY);
		case DEGREES180:
			return maxX - std::min(std::max(event->x(), 0), maxX);
		case DEGREES270:
			return std::min(std::max(event->y(), 0), maxY);
	}
}

int ZLQtViewWidget::ZLQtViewWidgetInternal::y(const QMouseEvent *event) const {
	const int maxX = width() - 1;
	const int maxY = height() + myHolder.myApplicationWindow.verticalAdjustment() - 1;
	switch (myHolder.rotation()) {
		default:
			return std::min(std::max(event->y(), 0), maxY);
		case DEGREES90:
			return std::min(std::max(event->x(), 0), maxX);
		case DEGREES180:
			return maxY - std::min(std::max(event->y(), 0), maxY);
		case DEGREES270:
			return maxX - std::min(std::max(event->x(), 0), maxX);
	}
}

void ZLQtViewWidget::repaint()	{
	myQWidget->repaint(false);
}
