/*
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
#include "QPaintContext.h"

#include <qpainter.h>
#include <qpixmap.h>

QViewWidget::QViewWidget(QWidget *parent, ZLApplication *application) : QWidget(parent), myApplication(application) {
	setBackgroundMode(NoBackground);
}

void QViewWidget::paintEvent(QPaintEvent*) {
	if (isRotated()) {
		((QPaintContext&)view()->context()).setSize(height(), width());
	} else {
		((QPaintContext&)view()->context()).setSize(width(), height());
	}
	view()->paint();
	QPainter realPainter(this);
	if (isRotated()) {
		realPainter.rotate(-90);
		realPainter.drawPixmap(1 - height(), -1, ((QPaintContext&)view()->context()).pixmap());
	} else {
		realPainter.drawPixmap(0, 0, ((QPaintContext&)view()->context()).pixmap());
	}
}

void QViewWidget::mousePressEvent(QMouseEvent *event) {
	if (isRotated()) {
		view()->onStylusPress(
			height() - event->y() - view()->context().rightMargin(),
			event->x() - view()->context().topMargin());
	} else {
		view()->onStylusPress(
			event->x() - view()->context().leftMargin(),
			event->y() - view()->context().topMargin());
	}
}

void QViewWidget::repaintView()	{
	repaint(false);
	myApplication->enableMenuButtons();
}
