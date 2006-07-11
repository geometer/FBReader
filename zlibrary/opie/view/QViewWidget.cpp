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

static QImage *myRotatedImage = 0;

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
	QPainter realPainter(this);
	const QPixmap &pixmap = ((QPaintContext&)myHolder.view()->context()).pixmap();
	switch (myHolder.rotation()) {
		default:
			if (myRotatedImage != 0) {
				delete myRotatedImage;
				myRotatedImage = 0;
			}
			realPainter.drawPixmap(0, 0, pixmap);
			break;
		case DEGREES90:
			if (myRotatedImage == 0) {
				myRotatedImage = new QImage(w, h, 16);
			}

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
				if (myRotatedImage != 0) {
					delete myRotatedImage;
					myRotatedImage = 0;
				}

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
			if (myRotatedImage == 0) {
				myRotatedImage = new QImage(w, h, 16);
			}

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
