/*
 * Copyright (C) 2004-2009 Geometer Plus <contact@geometerplus.com>
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

#include "ZLTextView.h"
#include "../area/ZLTextLineInfo.h"
#include "../area/ZLTextAreaStyle.h"

void ZLTextView::scrollPage(bool forward, ZLTextAreaController::ScrollingMode mode, unsigned int value) {
	preparePaintInfo();
	if (myTextAreaController.myPaintState == ZLTextAreaController::READY) {
		myTextAreaController.myPaintState = forward ? ZLTextAreaController::TO_SCROLL_FORWARD : ZLTextAreaController::TO_SCROLL_BACKWARD;
		myTextAreaController.myScrollingMode = mode;
		myTextAreaController.myOverlappingValue = value;
	}
}

void ZLTextView::preparePaintInfo() {
	size_t newWidth = 
		std::max(context().width() - leftMargin() - rightMargin(), 1);
	int viewHeight = context().height() - topMargin() - bottomMargin();
	shared_ptr<ZLTextPositionIndicatorInfo> indicatorInfo = this->indicatorInfo();
	if (!indicatorInfo.isNull() && (indicatorInfo->type() == ZLTextPositionIndicatorInfo::FB_INDICATOR)) {
		viewHeight -= indicatorInfo->height() + indicatorInfo->offset();
	}
	size_t newHeight = std::max(viewHeight, 1);

	if (newWidth != myTextAreaController.area().width() || newHeight != myTextAreaController.area().height()) {
		myTextAreaController.area().setSize(newWidth, newHeight);
		myTextAreaController.rebuildPaintInfo(false);
	}

	if (myTextAreaController.preparePaintInfo()) {
		myDoUpdateScrollbar = true;
	}
}
