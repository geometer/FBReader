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

#include "ZLView.h"
#include "ZLViewWidget.h"
#include "ZLPaintContext.h"

const ZLTypeId ZLView::TYPE_ID(ZLObjectWithRTTI::TYPE_ID);

const ZLTypeId &ZLView::typeId() const {
	return TYPE_ID;
}

ZLView::ZLView(ZLPaintContext &context) : myViewWidget(0), myContext(context) {
}

ZLView::~ZLView() {
}

ZLView::ScrollBarInfo::ScrollBarInfo() : Enabled(false), StandardLocation(true), Full(100), From(0), To(100) {
}

void ZLViewWidget::setView(shared_ptr<ZLView> view) {
	if (myView != 0) {
		myView->myViewWidget = 0;
	}
	myView = view;
	view->myViewWidget = this;
	rotate(myRotation);
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
	if (direction == VERTICAL) {
		myVerticalScrollbarInfo.Enabled = enabled;
	} else {
		myHorizontalScrollbarInfo.Enabled = enabled;
	}
	updateScrollbarState();
}

void ZLView::setScrollbarParameters(Direction direction, size_t full, size_t from, size_t to) {
	ScrollBarInfo &info = (direction == VERTICAL) ? myVerticalScrollbarInfo : myHorizontalScrollbarInfo;
	info.Full = full;
	info.From = from;
	info.To = to;
	updateScrollbarParameters();
}

void ZLView::updateScrollbarState() {
	if (myViewWidget == 0) {
		return;
	}
	switch (myViewWidget->rotation()) {
		case DEGREES0:
		case DEGREES180:
			myViewWidget->setScrollbarEnabled(VERTICAL, myVerticalScrollbarInfo.Enabled);
			myViewWidget->setScrollbarEnabled(HORIZONTAL, myHorizontalScrollbarInfo.Enabled);
			break;
		case DEGREES90:
		case DEGREES270:
			myViewWidget->setScrollbarEnabled(VERTICAL, myHorizontalScrollbarInfo.Enabled);
			myViewWidget->setScrollbarEnabled(HORIZONTAL, myVerticalScrollbarInfo.Enabled);
			break;
	}
}

void ZLView::updateScrollbarPlacement() {
	if (myViewWidget == 0) {
		return;
	}
	bool v = true;
	bool h = true;
	switch (myViewWidget->rotation()) {
		case DEGREES0:
			v = myVerticalScrollbarInfo.StandardLocation;
			h = myHorizontalScrollbarInfo.StandardLocation;
			break;
		case DEGREES90:
			v = myHorizontalScrollbarInfo.StandardLocation;
			h = !myVerticalScrollbarInfo.StandardLocation;
			break;
		case DEGREES180:
			v = !myVerticalScrollbarInfo.StandardLocation;
			h = !myHorizontalScrollbarInfo.StandardLocation;
			break;
		case DEGREES270:
			v = !myHorizontalScrollbarInfo.StandardLocation;
			h = myVerticalScrollbarInfo.StandardLocation;
			break;
	}
	myViewWidget->setScrollbarPlacement(VERTICAL, v);
	myViewWidget->setScrollbarPlacement(HORIZONTAL, h);
}

void ZLView::updateScrollbarParameters(Direction direction, const ScrollBarInfo &info, bool invert) {
	if (invert) {
		myViewWidget->setScrollbarParameters(
			direction,
			info.Full,
			info.Full - info.To,
			info.Full - info.From
		);
	} else {
		myViewWidget->setScrollbarParameters(
			direction,
			info.Full,
			info.From,
			info.To
		);
	}
}

void ZLView::updateScrollbarParameters() {		
	if (myViewWidget == 0) {
		return;
	}

	switch (myViewWidget->rotation()) {
		case DEGREES0:
			updateScrollbarParameters(VERTICAL, myVerticalScrollbarInfo, false);
			updateScrollbarParameters(HORIZONTAL, myHorizontalScrollbarInfo, false);
			break;
		case DEGREES90:
			updateScrollbarParameters(VERTICAL, myHorizontalScrollbarInfo, true);
			updateScrollbarParameters(HORIZONTAL, myVerticalScrollbarInfo, false);
			break;
		case DEGREES180:
			updateScrollbarParameters(VERTICAL, myVerticalScrollbarInfo, true);
			updateScrollbarParameters(HORIZONTAL, myHorizontalScrollbarInfo, true);
			break;
		case DEGREES270:
			updateScrollbarParameters(VERTICAL, myHorizontalScrollbarInfo, false);
			updateScrollbarParameters(HORIZONTAL, myVerticalScrollbarInfo, true);
			break;
	}
}

void ZLViewWidget::correctDirection(ZLView::Direction &direction, bool &invert) {
	switch (rotation()) {
		case ZLView::DEGREES0:
			invert = false;
			return;
		case ZLView::DEGREES90:
			invert = false;
			direction =
				(direction == ZLView::HORIZONTAL) ?
					ZLView::VERTICAL :
					ZLView::HORIZONTAL;
			return;
		case ZLView::DEGREES180:
			invert = true;
			return;
		case ZLView::DEGREES270:
			direction =
				(direction == ZLView::HORIZONTAL) ?
					ZLView::VERTICAL :
					ZLView::HORIZONTAL;
			invert = true;
			return;
	}
}

void ZLViewWidget::onScrollbarMoved(ZLView::Direction direction, size_t full, size_t from, size_t to) {
	if (!myView.isNull()) {
		bool invert = false;
		correctDirection(direction, invert);
		if (invert) {
			size_t tmp = full - from;
			from = full - to;
			to = tmp;
		}
		myView->onScrollbarMoved(direction, full, from, to);
	}
}

void ZLViewWidget::onScrollbarStep(ZLView::Direction direction, int steps) {
	if (!myView.isNull()) {
		bool invert = false;
		correctDirection(direction, invert);
		myView->onScrollbarStep(direction, invert ? -steps : steps);
	}
}

void ZLViewWidget::onScrollbarPageStep(ZLView::Direction direction, int steps) {
	if (!myView.isNull()) {
		bool invert = false;
		correctDirection(direction, invert);
		myView->onScrollbarPageStep(direction, invert ? -steps : steps);
	}
}

void ZLView::onScrollbarMoved(Direction, size_t, size_t, size_t) {
}

void ZLView::onScrollbarStep(Direction, int) {
}

void ZLView::onScrollbarPageStep(Direction, int) {
}

void ZLViewWidget::rotate(ZLView::Angle rotation) {
	myRotation = rotation;
	if (myView != 0) {
		myView->updateScrollbarState();
		myView->updateScrollbarPlacement();
		myView->updateScrollbarParameters();
	}
}

ZLView::Angle ZLViewWidget::rotation() const {
	return myRotation;
}
