/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "QFBReader.h"
#include "QViewWidget.h"
#include "QPaintContext.h"

#include <qpainter.h>
#include <qpixmap.h>
#include <qframe.h>

QViewWidget::QViewWidget(QWidget *parent) : QWidget(parent) {
	setBackgroundMode(NoBackground);
}

void QViewWidget::paintEvent(QPaintEvent*) {
	((QPaintContext&)view()->context()).setSize(width(), height());
	view()->paint();
	QPainter realPainter(this);
	realPainter.drawPixmap(0, 0, ((QPaintContext&)view()->context()).pixmap());
}

void QViewWidget::mousePressEvent(QMouseEvent *event) {
	view()->onStylusPress(
		event->x() - view()->context().leftMargin().value(),
		event->y() - view()->context().topMargin().value());
}

void QViewWidget::repaintView()	{
	repaint(false);
	((QFBReader*)parent())->enableMenuButtons();
}
