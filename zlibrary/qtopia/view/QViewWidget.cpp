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
#include "../application/QApplicationWindow.h"

QViewWidget::QViewWidgetInternal::QViewWidgetInternal(QWidget *parent, QViewWidget &holder) : QWidget(parent), myHolder(holder) {
	setBackgroundMode(NoBackground);
}

QViewWidget::QViewWidget(QWidget *parent, ZLApplication *application) : ZLViewWidget((ZLViewWidget::Angle)application->AngleStateOption.value()), myApplication(application) {
	myQWidget = new QViewWidgetInternal(parent, *this);
}

void QViewWidget::trackStylus(bool track) {
	myQWidget->setMouseTracking(track);
}

static QImage *myRotatedImage = 0;

void QViewWidget::QViewWidgetInternal::paintEvent(QPaintEvent*) {
	const int w = width();
	const int h = height() + ((QApplicationWindow*)parent())->veritcalAdjustment();
	switch (myHolder.rotation()) {
		default:
			((QPaintContext&)myHolder.view()->context()).setSize(w, h);
			if (myRotatedImage != 0) {
				delete myRotatedImage;
				myRotatedImage = 0;
			}
			break;
		case DEGREES90:
		case DEGREES270:
			((QPaintContext&)myHolder.view()->context()).setSize(h, w);
			if ((myRotatedImage != 0) && ((w != myRotatedImage->width()) || (h != myRotatedImage->height()))) {
				delete myRotatedImage;
				myRotatedImage = 0;
			}
			if (myRotatedImage == 0) {
				myRotatedImage = new QImage(w, h, 16);
			}
			break;
	}
	myHolder.view()->paint();
	QPainter realPainter(this);
	const QPixmap &pixmap = ((QPaintContext&)myHolder.view()->context()).pixmap();
	switch (myHolder.rotation()) {
		default:
			realPainter.drawPixmap(0, 0, pixmap);
			break;
		case DEGREES90:
			for (int i = 0; i < h; i++) {
				short *dataFrom = (short*)pixmap.scanLine(h - i - 1);
				short *dataTo = (short*)myRotatedImage->scanLine(i);
				for (int j = 0; j < w; j++) {
					dataTo[w - j - 1] = dataFrom[j];
				}
			}
			realPainter.drawImage(0, 0, *myRotatedImage);
			break;
		case DEGREES180:
			{
				short swap;
				int i = 0, j = w - 1;
				for (; i < j; i++, j--) {
					short *data0 = (short*)pixmap.scanLine(i);
					short *data1 = (short*)pixmap.scanLine(j);
					for (int k = 0; k < h; k++) {
						swap = data0[k];
						data0[k] = data1[h - k - 1];
						data1[h - k - 1] = swap;
					}
				}
				if (i == j) {
					short *data = (short*)pixmap.scanLine(i);
					for (int k = 0; k < h / 2; k++) {
						swap = data[k];
						data[k] = data[h - k - 1];
						data[h - k - 1] = swap;
					}
				}
			}
			realPainter.drawPixmap(0, 0, pixmap);
			break;
		case DEGREES270:
			for (int i = 0; i < h; i++) {
				memcpy(myRotatedImage->scanLine(i), pixmap.scanLine(i), 2 * w);
			}
			realPainter.drawImage(0, 0, *myRotatedImage);
			break;
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
	switch (myHolder.rotation()) {
		default:
			return std::min(std::max(event->x(), 0), width()) - myHolder.view()->context().leftMargin();
		case DEGREES90:
			return height() - std::min(std::max(event->y(), 0), height()) - myHolder.view()->context().rightMargin();
		case DEGREES180:
			return width() - std::min(std::max(event->x(), 0), width()) - myHolder.view()->context().rightMargin();
		case DEGREES270:
			return std::min(std::max(event->y(), 0), height()) - myHolder.view()->context().leftMargin();
	}
}

int QViewWidget::QViewWidgetInternal::y(const QMouseEvent *event) const {
	switch (myHolder.rotation()) {
		default:
			return std::min(std::max(event->y(), 0), height()) - myHolder.view()->context().topMargin();
		case DEGREES90:
			return std::min(std::max(event->x(), 0), width()) - myHolder.view()->context().topMargin();
		case DEGREES180:
			return height() - std::min(std::max(event->y(), 0), height()) - myHolder.view()->context().bottomMargin();
		case DEGREES270:
			return width() - std::min(std::max(event->x(), 0), width()) - myHolder.view()->context().bottomMargin();
	}
}

void QViewWidget::repaintView()	{
	myQWidget->repaint(false);
}
