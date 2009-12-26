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

#include <ZLUnicodeUtil.h>

#include "ZLTextView.h"
#include "ZLTextPositionIndicator.h"
#include "../area/ZLTextLineInfo.h"
#include "../area/ZLTextAreaStyle.h"
#include "../area/ZLTextSelectionModel.h"

void ZLTextView::paint() {
	myTextArea.setOffsets(
		myTextArea.isRtl() ? rightMargin() : leftMargin(), topMargin()
	);

	preparePaintInfo();

	myTextArea.myTextElementMap.clear();
	myTextArea.myTreeNodeMap.clear();
	myTextArea.context().clear(backgroundColor());

	if (textArea().isEmpty()) {
		return;
	}

	std::vector<size_t> labels;
	labels.reserve(myTextArea.myLineInfos.size() + 1);
	labels.push_back(0);

	ZLTextArea::Style style(textArea(), baseStyle());

	int y = 0;
	for (std::vector<ZLTextLineInfoPtr>::const_iterator it = myTextArea.myLineInfos.begin(); it != myTextArea.myLineInfos.end(); ++it) {
		const ZLTextLineInfo &info = **it;
		myTextArea.prepareTextLine(style, info, y);
		y += info.Height + info.Descent + info.VSpaceAfter;
		labels.push_back(myTextArea.myTextElementMap.size());
	}

	if (!isSelectionEnabled()) {
		myTextArea.selectionModel().clear();
	} else if (!mySelectionModelIsUpToDate) {
		myTextArea.selectionModel().update();
	}
	mySelectionModelIsUpToDate = true;

	y = 0;
	int index = 0;
	for (std::vector<ZLTextLineInfoPtr>::const_iterator it = myTextArea.myLineInfos.begin(); it != myTextArea.myLineInfos.end(); ++it) {
		const ZLTextLineInfo &info = **it;
		myTextArea.drawTextLine(style, info, y, labels[index], labels[index + 1]);
		y += info.Height + info.Descent + info.VSpaceAfter;
		++index;
	}

	shared_ptr<ZLTextPositionIndicatorInfo> indicatorInfo = this->indicatorInfo();
	if (!indicatorInfo.isNull() && (indicatorInfo->type() == ZLTextPositionIndicatorInfo::FB_INDICATOR)) {
		positionIndicator()->draw();
	}

	if (myDoUpdateScrollbar && !indicatorInfo.isNull()) {
		myDoUpdateScrollbar = false;
		const size_t full = positionIndicator()->sizeOfTextBeforeParagraph(positionIndicator()->endTextIndex());
		const size_t from = positionIndicator()->sizeOfTextBeforeCursor(textArea().startCursor());
		const size_t to = positionIndicator()->sizeOfTextBeforeCursor(textArea().endCursor());

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
