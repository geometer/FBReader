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

#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QMouseEvent>

ZLQtViewWidget::Widget::Widget(QWidget *parent, ZLQtViewWidget &holder) : QWidget(parent), myHolder(holder) {
	//setBackgroundMode(NoBackground);
}

ZLQtViewWidget::ZLQtViewWidget(QWidget *parent, ZLApplication *application) : ZLViewWidget((ZLView::Angle)application->AngleStateOption.value()), myApplication(application) {
	myQWidget = new Widget(parent, *this);
}

void ZLQtViewWidget::trackStylus(bool track) {
	myQWidget->setMouseTracking(track);
}

void ZLQtViewWidget::Widget::paintEvent(QPaintEvent*) {
	ZLQtPaintContext &context = (ZLQtPaintContext&)myHolder.view()->context();
	switch (myHolder.rotation()) {
		default:
			context.setSize(width(), height());
			break;
		case ZLView::DEGREES90:
		case ZLView::DEGREES270:
			context.setSize(height(), width());
			break;
	}
	myHolder.view()->paint();
	QPainter realPainter(this);
	switch (myHolder.rotation()) {
		default:
			realPainter.drawPixmap(0, 0, context.pixmap());
			break;
		case ZLView::DEGREES90:
			realPainter.rotate(270);
			realPainter.drawPixmap(1 - height(), -1, context.pixmap());
			break;
		case ZLView::DEGREES180:
			realPainter.rotate(180);
			realPainter.drawPixmap(1 - width(), 1 - height(), context.pixmap());
			break;
		case ZLView::DEGREES270:
			realPainter.rotate(90);
			realPainter.drawPixmap(-1, 1 - width(), context.pixmap());
			break;
	}
}

void ZLQtViewWidget::Widget::mousePressEvent(QMouseEvent *event) {
	myHolder.view()->onStylusMove(x(event), y(event));
	myHolder.view()->onStylusPress(x(event), y(event));
}

void ZLQtViewWidget::Widget::mouseReleaseEvent(QMouseEvent *event) {
	myHolder.view()->onStylusRelease(x(event), y(event));
}

void ZLQtViewWidget::Widget::mouseMoveEvent(QMouseEvent *event) {
	switch (event->buttons()) {
		case Qt::LeftButton:
			myHolder.view()->onStylusMovePressed(x(event), y(event));
			break;
		case Qt::NoButton:
			myHolder.view()->onStylusMove(x(event), y(event));
			break;
		default:
			break;
	}
}

int ZLQtViewWidget::Widget::x(const QMouseEvent *event) const {
	const int maxX = width() - 1;
	const int maxY = height() - 1;
	switch (myHolder.rotation()) {
		default:
			return std::min(std::max(event->x(), 0), maxX);
		case ZLView::DEGREES90:
			return maxY - std::min(std::max(event->y(), 0), maxY);
		case ZLView::DEGREES180:
			return maxX - std::min(std::max(event->x(), 0), maxX);
		case ZLView::DEGREES270:
			return std::min(std::max(event->y(), 0), maxY);
	}
}

int ZLQtViewWidget::Widget::y(const QMouseEvent *event) const {
	const int maxX = width() - 1;
	const int maxY = height() - 1;
	switch (myHolder.rotation()) {
		default:
			return std::min(std::max(event->y(), 0), maxY);
		case ZLView::DEGREES90:
			return std::min(std::max(event->x(), 0), maxX);
		case ZLView::DEGREES180:
			return maxY - std::min(std::max(event->y(), 0), maxY);
		case ZLView::DEGREES270:
			return maxX - std::min(std::max(event->x(), 0), maxX);
	}
}

void ZLQtViewWidget::repaint()	{
	myQWidget->repaint();
}

void ZLQtViewWidget::setScrollbarEnabled(ZLView::Direction direction, bool enabled) {
}

void ZLQtViewWidget::setScrollbarPlacement(ZLView::Direction direction, bool standard) {
}

void ZLQtViewWidget::setScrollbarParameters(ZLView::Direction direction, size_t full, size_t from, size_t to, size_t step) {
}
