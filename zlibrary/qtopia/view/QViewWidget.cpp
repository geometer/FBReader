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

#include "QViewWidget.h"
#include "QPaintContext.h"

QViewWidget::QViewWidget(QWidget *parent, ZLApplication *application) : QWidget(parent), myApplication(application) {
	setBackgroundMode(NoBackground);
}

void QViewWidget::trackStylus(bool track) {
	setMouseTracking(track);
}

void QViewWidget::paintEvent(QPaintEvent*) {
	((QPaintContext&)view()->context()).setSize(width(), height());
	view()->paint();
	QPainter realPainter(this);
	realPainter.drawPixmap(0, 0, ((QPaintContext&)view()->context()).pixmap());
}

void QViewWidget::mousePressEvent(QMouseEvent *event) {
	view()->onStylusPress(x(event), y(event));
}

void QViewWidget::mouseReleaseEvent(QMouseEvent *event) {
	view()->onStylusRelease(x(event), y(event));
}

void QViewWidget::mouseMoveEvent(QMouseEvent *event) {
	switch (event->state()) {
		case LeftButton:
			view()->onStylusMovePressed(x(event), y(event));
			break;
		case NoButton:
			view()->onStylusMove(x(event), y(event));
			break;
		default:
			break;
	}
}

int QViewWidget::x(const QMouseEvent *event) const {
	return std::min(std::max(event->x(), 0), width()) - view()->context().leftMargin();
}

int QViewWidget::y(const QMouseEvent *event) const {
	return std::min(std::max(event->y(), 0), height()) - view()->context().topMargin();
}

void QViewWidget::repaintView()	{
	repaint(false);
	myApplication->enableMenuButtons();
}
