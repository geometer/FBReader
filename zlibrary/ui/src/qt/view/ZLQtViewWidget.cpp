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

#include "ZLQtViewWidget.h"
#include "ZLQtPaintContext.h"

#include <qpainter.h>
#include <qpixmap.h>

ZLQtViewWidget::ZLQtViewWidgetInternal::ZLQtViewWidgetInternal(QWidget *parent, ZLQtViewWidget &holder) : QWidget(parent), myHolder(holder) {
	setBackgroundMode(NoBackground);
}

ZLQtViewWidget::ZLQtViewWidget(QWidget *parent, ZLApplication *application, const ZLQtViewWidgetPositionInfo &positionInfo) : ZLViewWidget((ZLViewWidget::Angle)application->AngleStateOption.value()), myApplication(application), myPositionInfo(positionInfo) {
	myQWidget = new ZLQtViewWidgetInternal(parent, *this);
}

const ZLQtViewWidgetPositionInfo &ZLQtViewWidget::positionInfo() const {
	return myPositionInfo;
}

void ZLQtViewWidget::trackStylus(bool track) {
	myQWidget->setMouseTracking(track);
}

void ZLQtViewWidget::ZLQtViewWidgetInternal::paintEvent(QPaintEvent*) {
	ZLQtPaintContext &context = (ZLQtPaintContext&)myHolder.view()->context();
	switch (myHolder.rotation()) {
		default:
			context.setSize(width(), height());
			break;
		case DEGREES90:
		case DEGREES270:
			context.setSize(height(), width());
			break;
	}
	myHolder.view()->paint();
	QPainter realPainter(this);
	switch (myHolder.rotation()) {
		default:
			realPainter.drawPixmap(0, 0, context.pixmap());
			break;
		case DEGREES90:
			realPainter.rotate(270);
			realPainter.drawPixmap(1 - height(), -1, context.pixmap());
			break;
		case DEGREES180:
			realPainter.rotate(180);
			realPainter.drawPixmap(1 - width(), 1 - height(), context.pixmap());
			break;
		case DEGREES270:
			realPainter.rotate(90);
			realPainter.drawPixmap(-1, 1 - width(), context.pixmap());
			break;
	}
}

void ZLQtViewWidget::ZLQtViewWidgetInternal::mousePressEvent(QMouseEvent *event) {
	myHolder.view()->onStylusMove(x(event), y(event));
	myHolder.view()->onStylusPress(x(event), y(event));
}

void ZLQtViewWidget::ZLQtViewWidgetInternal::mouseReleaseEvent(QMouseEvent *event) {
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
}

int ZLQtViewWidget::ZLQtViewWidgetInternal::x(const QMouseEvent *event) const {
	const int maxX = width() - 1;
	const int maxY = height() - 1;
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
	const int maxY = height() - 1;
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

void ZLQtViewWidget::ZLQtViewWidgetInternal::resizeEvent(QResizeEvent*) {
	const ZLQtViewWidgetPositionInfo &info = myHolder.positionInfo();

	const int x = info.x();
	const int y = info.y();
	const QPoint position = pos();
	if ((x != position.x()) || (y != position.y())) {
		move(x, y);
	}

	const int width = info.width();
	const int height = info.height();
	if ((width != this->width()) || (height != this->height())) {
		resize(width, height);
	}
}
