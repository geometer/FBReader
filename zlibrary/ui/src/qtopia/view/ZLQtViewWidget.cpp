/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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
#include <qlayout.h>
#include <qscrollbar.h>

#include <ZLibrary.h>
#include <ZLLanguageUtil.h>

#include "ZLQtViewWidget.h"
#include "ZLQtPaintContext.h"
#include "../application/ZLQtApplicationWindow.h"

ZLQtViewWidget::ZLQtViewWidgetInternal::ZLQtViewWidgetInternal(QWidget *parent, ZLQtViewWidget &holder) : QWidget(parent), myHolder(holder) {
	setBackgroundMode(NoBackground);
}

QScrollBar *ZLQtViewWidget::addScrollBar(QGridLayout *layout, Qt::Orientation orientation, int x, int y) {
	QScrollBar *scrollBar = new QScrollBar(orientation, myFrame);
	layout->addWidget(scrollBar, x, y);
	//scrollBar->hide();
	if (orientation == Qt::Vertical) {
		connect(scrollBar, SIGNAL(sliderMoved(int)), this, SLOT(onVerticalSliderMoved(int)));
		connect(scrollBar, SIGNAL(nextLine()), this, SLOT(onVerticalSliderStepNext()));
		connect(scrollBar, SIGNAL(nextPage()), this, SLOT(onVerticalSliderPageNext()));
		connect(scrollBar, SIGNAL(prevLine()), this, SLOT(onVerticalSliderStepPrevious()));
		connect(scrollBar, SIGNAL(prevPage()), this, SLOT(onVerticalSliderPagePrevious()));
	} else {
		connect(scrollBar, SIGNAL(sliderMoved(int)), this, SLOT(onHorizontalSliderMoved(int)));
		connect(scrollBar, SIGNAL(nextLine()), this, SLOT(onHorizontalSliderStepNext()));
		connect(scrollBar, SIGNAL(nextPage()), this, SLOT(onHorizontalSliderPageNext()));
		connect(scrollBar, SIGNAL(prevLine()), this, SLOT(onHorizontalSliderStepPrevious()));
		connect(scrollBar, SIGNAL(prevPage()), this, SLOT(onHorizontalSliderPagePrevious()));
	}
	return scrollBar;
}

ZLQtViewWidget::ZLQtViewWidget(QWidget *parent, ZLQtApplicationWindow &applicationWindow) : ZLViewWidget((ZLView::Angle)applicationWindow.application().AngleStateOption.value()), myApplicationWindow(applicationWindow) {
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

	/*
	myRightScrollBar = addScrollBar(layout, Qt::Vertical, 1, 2);
	myLeftScrollBar = addScrollBar(layout, Qt::Vertical, 1, 0);
	myShowScrollBarAtRight = true;

	myBottomScrollBar = addScrollBar(layout, Qt::Horizontal, 2, 1);
	myTopScrollBar = addScrollBar(layout, Qt::Horizontal, 0, 1);
	myShowScrollBarAtBottom = true;
	*/
}

void ZLQtViewWidget::trackStylus(bool track) {
	myQWidget->setMouseTracking(track);
}

void ZLQtViewWidget::ZLQtViewWidgetInternal::paintEvent(QPaintEvent*) {
	const int w = width();
	const int h = height() + myHolder.myApplicationWindow.verticalAdjustment();
	switch (myHolder.rotation()) {
		default:
			((ZLQtPaintContext&)myHolder.view()->context()).setSize(w, h);
			break;
		case ZLView::DEGREES90:
		case ZLView::DEGREES270:
			((ZLQtPaintContext&)myHolder.view()->context()).setSize(h, w);
			break;
	}
	myHolder.view()->paint();

	int angle = 0;
	switch (myHolder.rotation()) {
		default:
			break;
		case ZLView::DEGREES90:
			angle = 270;
			break;
		case ZLView::DEGREES180:
			angle = 180;
			break;
		case ZLView::DEGREES270:
			angle = 90;
			break;
	}
	const QPixmap &pixmap = ((ZLQtPaintContext&)myHolder.view()->context()).pixmap();
	QPainter painter(this);
	if (angle == 0) {
		painter.drawPixmap(0, 0, pixmap);
	} else {
		QWMatrix matrix;
		matrix.rotate(angle);
		painter.drawPixmap(0, 0, pixmap.xForm(matrix));
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
	const int maxY = height() + myHolder.myApplicationWindow.verticalAdjustment() - 1;
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
	const int maxY = height() + myHolder.myApplicationWindow.verticalAdjustment() - 1;
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

static void setShown(QWidget *widget, bool shown) {
	if (shown) {
		widget->show();
	} else {
		widget->hide();
	}
}

void ZLQtViewWidget::setScrollbarEnabled(ZLView::Direction direction, bool enabled) {
	return;
	if (direction == ZLView::VERTICAL) {
		setShown(myRightScrollBar, enabled && myShowScrollBarAtRight);
		setShown(myLeftScrollBar, enabled && !myShowScrollBarAtRight);
	} else {
		setShown(myBottomScrollBar, enabled && myShowScrollBarAtBottom);
		setShown(myTopScrollBar, enabled && !myShowScrollBarAtBottom);
	}
}

void ZLQtViewWidget::setScrollbarPlacement(ZLView::Direction direction, bool standard) {
	return;
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
			if (old->isVisible()) {
				old->hide();
				current->show();
			}
		}
	} else {
		if (standard != myShowScrollBarAtBottom) {
			myShowScrollBarAtBottom = standard;
			QScrollBar *old = standard ? myTopScrollBar : myBottomScrollBar;
			QScrollBar *current = standard ? myBottomScrollBar : myTopScrollBar;
			if (old->isVisible()) {
				old->hide();
				current->show();
			}
		}
	}
}

void ZLQtViewWidget::setScrollbarParameters(ZLView::Direction direction, size_t full, size_t from, size_t to) {
	return;
	QScrollBar *bar =
		(direction == ZLView::VERTICAL) ?  verticalScrollBar() : horizontalScrollBar();
	bar->setMinValue(0);
	bar->setMaxValue(full + from - to);
	bar->setValue(from);
	bar->setPageStep(to - from);
}

QWidget *ZLQtViewWidget::widget() const {
	return myFrame;
}

void ZLQtViewWidget::onVerticalSliderMoved(int value) {
	QScrollBar *bar = verticalScrollBar();
	size_t maxValue = bar->maxValue();
	size_t pageStep = bar->pageStep();
	onScrollbarMoved(
		ZLView::VERTICAL,
		maxValue + pageStep,
		(size_t)value,
		(size_t)value + pageStep
	);
}

void ZLQtViewWidget::onVerticalSliderStepNext() {
	onScrollbarStep(ZLView::VERTICAL, 1);
}

void ZLQtViewWidget::onVerticalSliderStepPrevious() {
	onScrollbarStep(ZLView::VERTICAL, -1);
}

void ZLQtViewWidget::onVerticalSliderPageNext() {
	onScrollbarPageStep(ZLView::VERTICAL, 1);
}

void ZLQtViewWidget::onVerticalSliderPagePrevious() {
	onScrollbarPageStep(ZLView::VERTICAL, -1);
}

void ZLQtViewWidget::onHorizontalSliderMoved(int value) {
	QScrollBar *bar = horizontalScrollBar();
	size_t maxValue = bar->maxValue();
	size_t pageStep = bar->pageStep();
	onScrollbarMoved(
		ZLView::HORIZONTAL,
		maxValue + pageStep,
		(size_t)value,
		(size_t)value + pageStep
	);
}

void ZLQtViewWidget::onHorizontalSliderStepNext() {
	onScrollbarStep(ZLView::HORIZONTAL, 1);
}

void ZLQtViewWidget::onHorizontalSliderStepPrevious() {
	onScrollbarStep(ZLView::HORIZONTAL, -1);
}

void ZLQtViewWidget::onHorizontalSliderPageNext() {
	onScrollbarPageStep(ZLView::HORIZONTAL, 1);
}

void ZLQtViewWidget::onHorizontalSliderPagePrevious() {
	onScrollbarPageStep(ZLView::HORIZONTAL, -1);
}

QScrollBar *ZLQtViewWidget::verticalScrollBar() {
	return myShowScrollBarAtRight ? myRightScrollBar : myLeftScrollBar;
}

QScrollBar *ZLQtViewWidget::horizontalScrollBar() {
	return myShowScrollBarAtBottom ? myBottomScrollBar : myTopScrollBar;
}
