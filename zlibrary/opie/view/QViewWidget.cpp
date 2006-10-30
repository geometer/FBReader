/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include "QViewWidget.h"
#include "QPaintContext.h"

QViewWidget::QViewWidgetInternal::QViewWidgetInternal(QWidget *parent, QViewWidget &holder) : QWidget(parent), myHolder(holder) {
	setBackgroundMode(NoBackground);
}

QViewWidget::QViewWidget(QWidget *parent, ZLApplication *application) : ZLViewWidget((ZLViewWidget::Angle)application->AngleStateOption.value()), myApplication(application) {
	myQWidget = new QViewWidgetInternal(parent, *this);
}

void QViewWidget::trackStylus(bool track) {
	myQWidget->setMouseTracking(track);
}

void QViewWidget::QViewWidgetInternal::paintEvent(QPaintEvent*) {
	const int w = width();
	const int h = height();
	switch (myHolder.rotation()) {
		default:
			((QPaintContext&)myHolder.view()->context()).setSize(w, h);
			break;
		case DEGREES90:
		case DEGREES270:
			((QPaintContext&)myHolder.view()->context()).setSize(h, w);
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
	const QPixmap &pixmap = ((QPaintContext&)myHolder.view()->context()).pixmap();
	QPainter painter(this);
	if (angle == 0) {
		painter.drawPixmap(0, 0, pixmap);
	} else {
		QWMatrix matrix;
		matrix.rotate(angle);
		painter.drawPixmap(0, 0, pixmap.xForm(matrix));
	}
}

void QViewWidget::QViewWidgetInternal::mousePressEvent(QMouseEvent *event) {
	myHolder.view()->onStylusPress(x(event), y(event));
}

void QViewWidget::QViewWidgetInternal::mouseReleaseEvent(QMouseEvent *event) {
	myHolder.view()->onStylusRelease(x(event), y(event));
}

void QViewWidget::QViewWidgetInternal::mouseMoveEvent(QMouseEvent *event) {
	switch (event->state()) {
		case LeftButton:
			myHolder.view()->onStylusMovePressed(x(event), y(event));
			break;
		case NoButton:
			myHolder.view()->onStylusMove(x(event), y(event));
			break;
		default:
			break;
	}
}

int QViewWidget::QViewWidgetInternal::x(const QMouseEvent *event) const {
	const int maxX = width() - 1;
	const int maxY = height() - 1;
	int logicalX;
	switch (myHolder.rotation()) {
		default:
			logicalX = std::min(std::max(event->x(), 0), maxX);
			break;
		case DEGREES90:
			logicalX = maxY - std::min(std::max(event->y(), 0), maxY);
			break;
		case DEGREES180:
			logicalX = maxX - std::min(std::max(event->x(), 0), maxX);
			break;
		case DEGREES270:
			logicalX = std::min(std::max(event->y(), 0), maxY);
			break;
	}
	return logicalX - myHolder.view()->context().leftMargin();
}

int QViewWidget::QViewWidgetInternal::y(const QMouseEvent *event) const {
	const int maxX = width() - 1;
	const int maxY = height() - 1;
	int logicalY;
	switch (myHolder.rotation()) {
		default:
			logicalY = std::min(std::max(event->y(), 0), maxY);
			break;
		case DEGREES90:
			logicalY = std::min(std::max(event->x(), 0), maxX);
			break;
		case DEGREES180:
			logicalY = maxY - std::min(std::max(event->y(), 0), maxY);
			break;
		case DEGREES270:
			logicalY = maxX - std::min(std::max(event->x(), 0), maxX);
			break;
	}
	return logicalY - myHolder.view()->context().topMargin();
}

void QViewWidget::repaint()	{
	myQWidget->repaint(false);
}
