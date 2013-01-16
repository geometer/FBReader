/*
 * Copyright (C) 2004-2013 Geometer Plus <contact@geometerplus.com>
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

#include <ZLColor.h>

#include "ZLTextView.h"
#include "ZLTextPositionIndicator.h"

int ZLTextView::headerHeight() const {
	return 0;
}

void ZLTextView::paintHeader() const {
}

void ZLTextView::paint() {
	context().clear(backgroundColor());

	myTextAreaController.area().setOffsets(
		textArea().isRtl() ? rightMargin() : leftMargin(), topMargin() + headerHeight()
	);

	preparePaintInfo();

	if (textArea().isEmpty()) {
		return;
	}

	myTextAreaController.area().paint();

	shared_ptr<ZLTextPositionIndicatorInfo> indicatorInfo = this->indicatorInfo();
	if (!indicatorInfo.isNull()) {
		switch (indicatorInfo->type()) {
			default:
				break;
			case ZLTextPositionIndicatorInfo::PAGE_FOOTER:
				positionIndicator()->draw();
				break;
			case ZLTextPositionIndicatorInfo::PAGE_HEADER:
				paintHeader();
				break;
		}
	}

	if (myDoUpdateScrollbar && !indicatorInfo.isNull()) {
		myDoUpdateScrollbar = false;
		const std::size_t full = sizeOfTextBeforeParagraph(endTextIndex());
		const std::size_t from = sizeOfTextBeforeCursor(textArea().startCursor());
		const std::size_t to = sizeOfTextBeforeCursor(textArea().endCursor());

		bool showScrollbar =
			(indicatorInfo->type() == ZLTextPositionIndicatorInfo::OS_SCROLLBAR) &&
			(to - from < full);
		if (showScrollbar) {
			setScrollbarEnabled(VERTICAL, true);
			setScrollbarParameters(VERTICAL, full, from, to);
		} else {
			setScrollbarEnabled(VERTICAL, false);
		}
	}

	ZLTextParagraphCursorCache::cleanup();
}
