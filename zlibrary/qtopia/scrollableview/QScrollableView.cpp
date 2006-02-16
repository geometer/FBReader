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

#include <qpe/resource.h>

#include <qpainter.h>
#include <qpixmap.h>
#include <qscrollview.h>
#include <qtimer.h>

#include "QScrollableView.h"

QScrollableView::QScrollableView(QWidget *parent) : QWidget(parent) {
	setBackgroundMode(NoBackground);

	myScrollWidget = new QScrollView(parent);
	myScrollWidget->addChild(this);
	myScrollWidget->setVScrollBarMode(QScrollView::Auto);
	myScrollWidget->setHScrollBarMode(QScrollView::Auto);

	myDoubleBufferPixmap = NULL;
	myDoRealRepaint = true;

	myTimer = NULL;
}

QScrollableView::~QScrollableView() {
	if (myDoubleBufferPixmap != NULL) {
		delete myDoubleBufferPixmap;
	}
	if (myTimer != NULL) {
		delete myTimer;
	}
}

void QScrollableView::startTimer(int mseconds) {
	if (myTimer == NULL) {
		myTimer = new QTimer();
		connect(myTimer, SIGNAL(timeout()), this, SLOT(onTimerTick()));
	} else {
		myTimer->stop();
	}
	myTimer->start(mseconds);
}

void QScrollableView::onTimerTick() {
	viewable().onTimerTick();
}

void QScrollableView::repaintView() {
	myDoRealRepaint = true;
	repaint(false);
}

void QScrollableView::clearView() {
	if (myDoubleBufferPixmap != NULL) {
		delete myDoubleBufferPixmap;
		myDoubleBufferPixmap = NULL;
	}
}

void QScrollableView::paintEvent(QPaintEvent *) {
	if (myDoubleBufferPixmap != NULL) {
		if ((myDoubleBufferPixmap->width() != width()) || (myDoubleBufferPixmap->height() != height())) {
			delete myDoubleBufferPixmap;
			myDoubleBufferPixmap = NULL;
		}
	}
	if (myDoubleBufferPixmap == NULL) {
		myDoubleBufferPixmap = new QPixmap(width(), height());
		myDoRealRepaint = true;
	}
	if (myDoRealRepaint) {
		viewable().doRealPaint();
		myDoRealRepaint = false;
	}

	QPainter painter(this);
	// TODO: optimize (use event parameter)
	painter.drawPixmap(0, 0, *myDoubleBufferPixmap);
}

void QScrollableView::mousePressEvent(QMouseEvent *event) {
	viewable().onStylusPress(event->x(), event->y());
}

void QScrollableView::mouseReleaseEvent(QMouseEvent *event) {
	viewable().onStylusRelease(event->x(), event->y());
}

void QScrollableView::mouseMoveEvent(QMouseEvent *event) {
	viewable().onStylusMove(event->x(), event->y());
}

void QScrollableView::mouseDoubleClickEvent(QMouseEvent *event) {
	viewable().onStylusDoubleClick(event->x(), event->y());
}

int QScrollableView::visibleAreaWidth(bool scrollBarHidden) const {
	int width = myScrollWidget->width() - style().defaultFrameWidth() - 2;
	if (!scrollBarHidden) {
		// TODO: avoid this hack
		//width -= myScrollWidget->verticalScrollBar()->width();
		int delta = myScrollWidget->verticalScrollBar()->width();
		if (delta > 32) {
			delta = 32;
		}
		width -= delta;
	}
	return width;
}

int QScrollableView::visibleAreaHeight(bool scrollBarHidden) const {
	int height = myScrollWidget->height() - style().defaultFrameWidth() - 2;
	if (!scrollBarHidden) {
		height -= myScrollWidget->horizontalScrollBar()->height();
	}
	return height;
}

void QScrollableView::resizeView(int w, int h) {
	if ((w != width()) || (h != height())) {
		resize(w, h);
	}
}
