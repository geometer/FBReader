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

#include "ZLView.h"
#include "ZLViewWidget.h"
#include "ZLPaintContext.h"
#include "../application/ZLApplication.h"

ZLView::ZLView(ZLApplication &application, shared_ptr<ZLPaintContext> context) : myApplication(application), myViewWidget(0), myContext(context) {
}

ZLView::~ZLView() {
}

void ZLView::setPaintContext(shared_ptr<ZLPaintContext> context) {
	myContext = context;
}

void ZLViewWidget::setView(shared_ptr<ZLView> view) {
	if (myView != 0) {
		myView->myViewWidget = 0;
	}
	myView = view;
	view->myViewWidget = this;
}

bool ZLView::onStylusPress(int, int) {
	return false;
}

bool ZLView::onStylusRelease(int, int) {
	return false;
}

bool ZLView::onStylusMove(int, int) {
	return false;
}

bool ZLView::onStylusMovePressed(int, int) {
	return false;
}

bool ZLView::onFingerTap(int, int) {
	return false;
}

void ZLView::setScrollbarEnabled(Direction direction, bool enabled) {
	if (myViewWidget != 0) {
		myViewWidget->setScrollbarEnabled(direction, enabled);
	}
}

void ZLView::setScrollbarPlacement(Direction direction, bool standard) {
	if (myViewWidget != 0) {
		myViewWidget->setScrollbarPlacement(direction, standard);
	}
}

void ZLView::setScrollbarParameters(Direction direction, size_t full, size_t from, size_t to, size_t step) {
	if (myViewWidget != 0) {
		myViewWidget->setScrollbarParameters(direction, full, from, to, step);
	}
}

void ZLViewWidget::onScrollbarMoved(ZLView::Direction direction, size_t full, size_t from, size_t to) {
	if (!myView.isNull()) {
		myView->onScrollbarMoved(direction, full, from, to);
	}
}

void ZLView::onScrollbarMoved(Direction, size_t, size_t, size_t) {
}

void ZLViewWidget::rotate(ZLView::Angle rotation) {
	myRotation = rotation;
}

ZLView::Angle ZLViewWidget::rotation() const {
	return myRotation;
}

ZLView::Angle ZLView::rotation() const {
	return (myViewWidget != 0) ? myViewWidget->rotation() : DEGREES0;
}
