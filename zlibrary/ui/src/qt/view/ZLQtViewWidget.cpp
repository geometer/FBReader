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

#include <qpainter.h>
#include <qpixmap.h>
#include <qlayout.h>
#include <qframe.h>
#include <qscrollbar.h>

#include <ZLibrary.h>
#include <ZLLanguageUtil.h>

#include "ZLQtViewWidget.h"
#include "ZLQtPaintContext.h"

ZLQtViewWidget::ZLQtViewWidgetInternal::ZLQtViewWidgetInternal(QWidget *parent, ZLQtViewWidget &holder) : QWidget(parent), myHolder(holder) {
	setBackgroundMode(NoBackground);
}

QScrollBar *ZLQtViewWidget::addScrollBar(QGridLayout *layout, Qt::Orientation orientation, int x, int y, const char *slot) {
	QScrollBar *scrollBar = new QScrollBar(orientation, myFrame);
	layout->addWidget(scrollBar, x, y);
	scrollBar->hide();
	connect(scrollBar, SIGNAL(sliderMoved(int)), this, slot);
	return scrollBar;
}

ZLQtViewWidget::ZLQtViewWidget(QWidget *parent, ZLApplication *application, const ZLQtViewWidgetPositionInfo &positionInfo) : ZLViewWidget((ZLView::Angle)application->AngleStateOption.value()), myApplication(application), myPositionInfo(positionInfo) {
	myFrame = new QFrame(parent);
	QGridLayout *layout = new QGridLayout(myFrame, 3, 3);
	layout->setRowStretch(0, 0);
	layout->setRowStretch(1, 1);
	layout->setRowStretch(2, 0);
	layout->setColStretch(0, 0);
	layout->setColStretch(1, 1);
	layout->setColStretch(2, 0);
	myQWidget = new ZLQtViewWidgetInternal(myFrame, *this);
  layout->addWidget(myQWidget, 1, 1);

	myRightScrollBar = addScrollBar(layout, Qt::Vertical, 1, 2, SLOT(onVerticalSliderMoved(int)));
	myLeftScrollBar = addScrollBar(layout, Qt::Vertical, 1, 0, SLOT(onVerticalSliderMoved(int)));
	myShowScrollBarAtRight = true;

	myBottomScrollBar = addScrollBar(layout, Qt::Horizontal, 2, 1, SLOT(onHorizontalSliderMoved(int)));
	myTopScrollBar = addScrollBar(layout, Qt::Horizontal, 0, 1, SLOT(onHorizontalSliderMoved(int)));
	myShowScrollBarAtBottom = true;
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
			realPainter.drawPixmap(0, 1 - width(), context.pixmap());
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
		case ZLView::DEGREES90:
			return maxY - std::min(std::max(event->y(), 0), maxY);
		case ZLView::DEGREES180:
			return maxX - std::min(std::max(event->x(), 0), maxX);
		case ZLView::DEGREES270:
			return std::min(std::max(event->y(), 0), maxY);
	}
}

int ZLQtViewWidget::ZLQtViewWidgetInternal::y(const QMouseEvent *event) const {
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
	myQWidget->repaint(false);
}

void ZLQtViewWidget::ZLQtViewWidgetInternal::resizeEvent(QResizeEvent*) {
	/*
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
	*/
}

void ZLQtViewWidget::setScrollbarEnabled(ZLView::Direction direction, bool enabled) {
	if (direction == ZLView::VERTICAL) {
		myRightScrollBar->setShown(enabled && myShowScrollBarAtRight);
		myLeftScrollBar->setShown(enabled && !myShowScrollBarAtRight);
	} else {
		myBottomScrollBar->setShown(enabled && myShowScrollBarAtBottom);
		myTopScrollBar->setShown(enabled && !myShowScrollBarAtBottom);
	}
}

void ZLQtViewWidget::setScrollbarPlacement(ZLView::Direction direction, bool standard) {
	if ((rotation() == ZLView::DEGREES90) || (rotation() == ZLView::DEGREES270)) {
		if (ZLLanguageUtil::isRTLLanguage(ZLibrary::Language())) {
			standard = !standard;
		}
	}
	if (direction == ZLView::VERTICAL) {
		if (standard != myShowScrollBarAtRight) {
			myShowScrollBarAtRight = standard;
			QScrollBar *old = standard ? myLeftScrollBar : myRightScrollBar;
			QScrollBar *current = standard ? myRightScrollBar : myLeftScrollBar;
			if (old->isShown()) {
				old->hide();
				current->show();
			}
		}
	} else {
		if (standard != myShowScrollBarAtBottom) {
			myShowScrollBarAtBottom = standard;
			QScrollBar *old = standard ? myTopScrollBar : myBottomScrollBar;
			QScrollBar *current = standard ? myBottomScrollBar : myTopScrollBar;
			if (old->isShown()) {
				old->hide();
				current->show();
			}
		}
	}
}

void ZLQtViewWidget::setScrollbarParameters(ZLView::Direction direction, size_t full, size_t from, size_t to, size_t step) {
	QScrollBar *bar =
		(direction == ZLView::VERTICAL) ?
			(myShowScrollBarAtRight ? myRightScrollBar : myLeftScrollBar) :
			(myShowScrollBarAtBottom ? myBottomScrollBar : myTopScrollBar);
	bar->setMinValue(0);
	bar->setMaxValue(full + from - to);
	bar->setValue(from);
	bar->setPageStep(to - from);
}

QWidget *ZLQtViewWidget::widget() const {
	return myFrame;
}

void ZLQtViewWidget::onVerticalSliderMoved(int value) {
	QScrollBar *bar =
		myShowScrollBarAtRight ? myRightScrollBar : myLeftScrollBar;
	size_t maxValue = bar->maxValue();
	size_t pageStep = bar->pageStep();
	onScrollbarMoved(
		ZLView::VERTICAL,
		maxValue + pageStep,
		(size_t)value,
		(size_t)value + pageStep
	);
}

void ZLQtViewWidget::onHorizontalSliderMoved(int value) {
	QScrollBar *bar =
		myShowScrollBarAtBottom ? myBottomScrollBar : myTopScrollBar;
	size_t maxValue = bar->maxValue();
	size_t pageStep = bar->pageStep();
	onScrollbarMoved(
		ZLView::HORIZONTAL,
		maxValue + pageStep,
		(size_t)value,
		(size_t)value + pageStep
	);
}
