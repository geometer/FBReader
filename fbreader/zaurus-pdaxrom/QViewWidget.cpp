/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "QViewWidget.h"
#include "QFBReader.h"
#include "QPaintContext.h"

#include <qpainter.h>
#include <qpixmap.h>

QViewWidget::QViewWidget(QWidget *parent) : QWidget(parent) {
	setBackgroundMode(NoBackground);
	myIsRotated = false;
}

void QViewWidget::setRotated(bool rotated) {
	myIsRotated = rotated;
}

void QViewWidget::paintEvent(QPaintEvent*) {
	if (myIsRotated) {
		((QPaintContext&)view()->context()).setSize(height(), width());
	} else {
		((QPaintContext&)view()->context()).setSize(width(), height());
	}
	view()->paint();
	QPainter realPainter(this);
	if (myIsRotated) {
		realPainter.rotate(-90);
		realPainter.drawPixmap(1 - height(), -1, ((QPaintContext&)view()->context()).pixmap());
	} else {
		realPainter.drawPixmap(0, 0, ((QPaintContext&)view()->context()).pixmap());
	}
}

void QViewWidget::mousePressEvent(QMouseEvent *event) {
	if (myIsRotated) {
		view()->onStylusPress(
			height() - event->y() - view()->context().rightMargin().value(),
			event->x() - view()->context().topMargin().value());
	} else {
		view()->onStylusPress(
			event->x() - view()->context().leftMargin().value(),
			event->y() - view()->context().topMargin().value());
	}
}

void QViewWidget::repaintView()	{
	repaint(false);
	((QFBReader*)parent())->enableMenuButtons();
}
