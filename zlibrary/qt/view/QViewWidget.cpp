/*
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "QViewWidget.h"
#include "QPaintContext.h"

#include <qpainter.h>
#include <qpixmap.h>

QViewWidget::QViewWidget(QWidget *parent, ZLApplication *application) : QWidget(parent), myApplication(application) {
	setBackgroundMode(NoBackground);
}

void QViewWidget::trackStylus(bool track) {
	setMouseTracking(track);
}

void QViewWidget::paintEvent(QPaintEvent*) {
	switch (rotation()) {
		default:
			((QPaintContext&)view()->context()).setSize(width(), height());
			break;
		case DEGREES90:
		case DEGREES270:
			((QPaintContext&)view()->context()).setSize(height(), width());
			break;
	}
	view()->paint();
	QPainter realPainter(this);
	switch (rotation()) {
		default:
			realPainter.drawPixmap(0, 0, ((QPaintContext&)view()->context()).pixmap());
			break;
		case DEGREES90:
			realPainter.rotate(90);
			realPainter.drawPixmap(-1, 1 - width(), ((QPaintContext&)view()->context()).pixmap());
			break;
		case DEGREES180:
			realPainter.rotate(180);
			realPainter.drawPixmap(1 - width(), 1 - height(), ((QPaintContext&)view()->context()).pixmap());
			break;
		case DEGREES270:
			realPainter.rotate(270);
			realPainter.drawPixmap(1 - height(), -1, ((QPaintContext&)view()->context()).pixmap());
			break;
	}
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
	switch (rotation()) {
		default:
			return std::min(std::max(event->x(), 0), width()) - view()->context().leftMargin();
		case DEGREES90:
			return std::min(std::max(event->y(), 0), height()) - view()->context().leftMargin();
		case DEGREES180:
			return width() - std::min(std::max(event->x(), 0), width()) - view()->context().rightMargin();
		case DEGREES270:
			return height() - std::min(std::max(event->y(), 0), height()) - view()->context().rightMargin();
	}
}

int QViewWidget::y(const QMouseEvent *event) const {
	switch (rotation()) {
		default:
			return std::min(std::max(event->y(), 0), height()) - view()->context().topMargin();
		case DEGREES90:
			return width() - std::min(std::max(event->x(), 0), width()) - view()->context().bottomMargin();
		case DEGREES180:
			return height() - std::min(std::max(event->y(), 0), height()) - view()->context().bottomMargin();
		case DEGREES270:
			return std::min(std::max(event->x(), 0), width()) - view()->context().topMargin();
	}
}

void QViewWidget::repaintView()	{
	repaint(false);
	myApplication->enableMenuButtons();
}
