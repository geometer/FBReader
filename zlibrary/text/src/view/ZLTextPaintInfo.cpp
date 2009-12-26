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

void ZLTextView::rebuildPaintInfo(bool strong) {
	if (myPaintState == NOTHING_TO_PAINT) {
		return;
	}

	if ((myPaintState == TO_SCROLL_FORWARD) || (myPaintState == TO_SCROLL_BACKWARD)) {
		preparePaintInfo();
	}

	myTextArea.myLineInfos.clear();
	if (strong) {
		ZLTextParagraphCursorCache::clear();
		myTextArea.myLineInfoCache.clear();
	}

	if (!myTextArea.myStartCursor.isNull()) {
		if (strong) {
			myTextArea.myStartCursor.rebuild();
		}
		myTextArea.myEndCursor = 0;
		myPaintState = START_IS_KNOWN;
	} else if (!myTextArea.myEndCursor.isNull()) {
		if (strong) {
			myTextArea.myEndCursor.rebuild();
		}
		myPaintState = END_IS_KNOWN;
	}
}

void ZLTextView::moveStartCursor(int paragraphIndex, int elementIndex, int charIndex) {
	if (myPaintState == NOTHING_TO_PAINT) {
		return;
	}

	if (myTextArea.myStartCursor.isNull()) {
		myTextArea.myStartCursor = myTextArea.myEndCursor;
	}
	myTextArea.myStartCursor.moveToParagraph(paragraphIndex);
	myTextArea.myStartCursor.moveTo(elementIndex, charIndex);
	myTextArea.myEndCursor = 0;
	myTextArea.myLineInfos.clear();
	myPaintState = START_IS_KNOWN;
}

void ZLTextView::moveEndCursor(int paragraphIndex, int elementIndex, int charIndex) {
	if (myPaintState == NOTHING_TO_PAINT) {
		return;
	}

	if (myTextArea.myEndCursor.isNull()) {
		myTextArea.myEndCursor = myTextArea.myStartCursor;
	}
	myTextArea.myEndCursor.moveToParagraph(paragraphIndex);
	if ((paragraphIndex > 0) && (elementIndex == 0) && (charIndex == 0)) {
		myTextArea.myEndCursor.previousParagraph();
		myTextArea.myEndCursor.moveToParagraphEnd();
	} else {
		myTextArea.myEndCursor.moveTo(elementIndex, charIndex);
	}
	myTextArea.myStartCursor = 0;
	myTextArea.myLineInfos.clear();
	myPaintState = END_IS_KNOWN;
}

ZLTextWordCursor ZLTextView::findLineFromStart(unsigned int overlappingValue) const {
	if (myTextArea.myLineInfos.empty() || (overlappingValue == 0)) {
		return ZLTextWordCursor();
	}

	std::vector<ZLTextLineInfoPtr>::const_iterator it;
	for (it = myTextArea.myLineInfos.begin(); it != myTextArea.myLineInfos.end(); ++it) {
		if ((*it)->IsVisible) {
			--overlappingValue;
			if (overlappingValue == 0) {
				break;
			}
		}
	}
	return (it != myTextArea.myLineInfos.end()) ? (*it)->End : (*myTextArea.myLineInfos.back()).End;
}

ZLTextWordCursor ZLTextView::findLineFromEnd(unsigned int overlappingValue) const {
	if (myTextArea.myLineInfos.empty() || (overlappingValue == 0)) {
		return ZLTextWordCursor();
	}

	std::vector<ZLTextLineInfoPtr>::const_iterator it;
	for (it = myTextArea.myLineInfos.end() - 1; it != myTextArea.myLineInfos.begin(); --it) {
		if ((*it)->IsVisible) {
			--overlappingValue;
			if (overlappingValue == 0) {
				break;
			}
		}
	}
	return (*it)->Start;
}

ZLTextWordCursor ZLTextView::findPercentFromStart(unsigned int percent) const {
	if (myTextArea.myLineInfos.empty()) {
		return ZLTextWordCursor();
	}

	int height = textHeight() * percent / 100;
	bool visibleLineOccured = false;
	std::vector<ZLTextLineInfoPtr>::const_iterator it;
	for (it = myTextArea.myLineInfos.begin(); it != myTextArea.myLineInfos.end(); ++it) {
		const ZLTextLineInfo &info = **it;
		if (info.IsVisible) {
			visibleLineOccured = true;
		}
		height -= info.Height + info.Descent + info.VSpaceAfter;
		if (visibleLineOccured && (height <= 0)) {
			break;
		}
	}
	return (it != myTextArea.myLineInfos.end()) ? (*it)->End : (*myTextArea.myLineInfos.back()).End;
}

void ZLTextView::preparePaintInfo() {
	size_t newWidth = 
		std::max(context().width() - leftMargin() - rightMargin(), 1);
	size_t newHeight = textHeight();
	if (newWidth != myTextArea.width() || newHeight != myTextArea.height()) {
		myTextArea.setSize(newWidth, newHeight);
		rebuildPaintInfo(false);
	}

	if ((myPaintState == NOTHING_TO_PAINT) || (myPaintState == READY)) {
		return;
	}
	myDoUpdateScrollbar = true;

	myTextArea.myLineInfoCache.insert(myTextArea.myLineInfos.begin(), myTextArea.myLineInfos.end());

	switch (myPaintState) {
		default:
			break;
		case TO_SCROLL_FORWARD:
			if (!myTextArea.myEndCursor.paragraphCursor().isLast() || !myTextArea.myEndCursor.isEndOfParagraph()) {
				ZLTextWordCursor startCursor;
				switch (myScrollingMode) {
					case NO_OVERLAPPING:
						break;
					case KEEP_LINES:
						startCursor = findLineFromEnd(myOverlappingValue);
						break;
					case SCROLL_LINES:
						startCursor = findLineFromStart(myOverlappingValue);
						if (startCursor.isEndOfParagraph()) {
							startCursor.nextParagraph();
						}
						break;
					case SCROLL_PERCENTAGE:
						startCursor = findPercentFromStart(myOverlappingValue);
						break;
				}
			
				if (!startCursor.isNull() && (startCursor == myTextArea.myStartCursor)) {
					startCursor = findLineFromStart(1);
				}

				if (!startCursor.isNull()) {
					ZLTextWordCursor endCursor = myTextArea.buildInfos(startCursor);
					if (!myTextArea.visiblePageIsEmpty() && ((myScrollingMode != KEEP_LINES) || (endCursor != myTextArea.myEndCursor))) {
						myTextArea.myStartCursor = startCursor;
						myTextArea.myEndCursor = endCursor;
						break;
					}
				}
				myTextArea.myStartCursor = myTextArea.myEndCursor;
				myTextArea.myEndCursor = myTextArea.buildInfos(myTextArea.myStartCursor);
			}
			break;
		case TO_SCROLL_BACKWARD:
			if (!myTextArea.myStartCursor.paragraphCursor().isFirst() || !myTextArea.myStartCursor.isStartOfParagraph()) {
				switch (myScrollingMode) {
					case NO_OVERLAPPING:
						myTextArea.myStartCursor = myTextArea.findStart(myTextArea.myStartCursor, ZLTextArea::PIXEL_UNIT, textHeight());
						break;
					case KEEP_LINES:
					{
						ZLTextWordCursor endCursor = findLineFromStart(myOverlappingValue);
						if (!endCursor.isNull() && (endCursor == myTextArea.myEndCursor)) {
							endCursor = findLineFromEnd(1);
						}
						if (!endCursor.isNull()) {
							ZLTextWordCursor startCursor = myTextArea.findStart(endCursor, ZLTextArea::PIXEL_UNIT, textHeight());
							myTextArea.myStartCursor =
								(startCursor != myTextArea.myStartCursor) ?
									startCursor : myTextArea.findStart(myTextArea.myStartCursor, ZLTextArea::PIXEL_UNIT, textHeight());
						} else {
							myTextArea.myStartCursor = myTextArea.findStart(myTextArea.myStartCursor, ZLTextArea::PIXEL_UNIT, textHeight());
						}
						break;
					}
					case SCROLL_LINES:
						myTextArea.myStartCursor = myTextArea.findStart(myTextArea.myStartCursor, ZLTextArea::LINE_UNIT, myOverlappingValue);
						break;
					case SCROLL_PERCENTAGE:
						myTextArea.myStartCursor =
							myTextArea.findStart(myTextArea.myStartCursor, ZLTextArea::PIXEL_UNIT, textHeight() * myOverlappingValue / 100);
						break;
				}
				myTextArea.myEndCursor = myTextArea.buildInfos(myTextArea.myStartCursor);
				if (myTextArea.visiblePageIsEmpty()) {
					myTextArea.myStartCursor = myTextArea.findStart(myTextArea.myStartCursor, ZLTextArea::LINE_UNIT, 1);
					myTextArea.myEndCursor = myTextArea.buildInfos(myTextArea.myStartCursor);
				}
			}
			break;
		case START_IS_KNOWN:
			myTextArea.myEndCursor = myTextArea.buildInfos(myTextArea.myStartCursor);
			if (myTextArea.visiblePageIsEmpty()) {
				ZLTextWordCursor startCursor = findLineFromStart(1);
				if (!startCursor.isNull()) {
					myTextArea.myStartCursor = startCursor;
					if (myTextArea.myStartCursor.isEndOfParagraph()) {
						myTextArea.myStartCursor.nextParagraph();
					}
					myTextArea.myEndCursor = myTextArea.buildInfos(myTextArea.myStartCursor);
				}
			}
			break;
		case END_IS_KNOWN:
			myTextArea.myStartCursor = myTextArea.findStart(myTextArea.myEndCursor, ZLTextArea::PIXEL_UNIT, textHeight());
			myTextArea.myEndCursor = myTextArea.buildInfos(myTextArea.myStartCursor);
			break;
	}
	myPaintState = READY;
	myTextArea.myLineInfoCache.clear();
}
