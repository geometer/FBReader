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

bool ZLTextView::pageIsEmpty() const {
	for (std::vector<ZLTextLineInfoPtr>::const_iterator it = myTextArea.myLineInfos.begin(); it != myTextArea.myLineInfos.end(); ++it) {
		if ((*it)->IsVisible) {
			return false;
		}
	}
	return true;
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
					ZLTextWordCursor endCursor = buildInfos(startCursor);
					if (!pageIsEmpty() && ((myScrollingMode != KEEP_LINES) || (endCursor != myTextArea.myEndCursor))) {
						myTextArea.myStartCursor = startCursor;
						myTextArea.myEndCursor = endCursor;
						break;
					}
				}
				myTextArea.myStartCursor = myTextArea.myEndCursor;
				myTextArea.myEndCursor = buildInfos(myTextArea.myStartCursor);
			}
			break;
		case TO_SCROLL_BACKWARD:
			if (!myTextArea.myStartCursor.paragraphCursor().isFirst() || !myTextArea.myStartCursor.isStartOfParagraph()) {
				switch (myScrollingMode) {
					case NO_OVERLAPPING:
						myTextArea.myStartCursor = findStart(myTextArea.myStartCursor, PIXEL_UNIT, textHeight());
						break;
					case KEEP_LINES:
					{
						ZLTextWordCursor endCursor = findLineFromStart(myOverlappingValue);
						if (!endCursor.isNull() && (endCursor == myTextArea.myEndCursor)) {
							endCursor = findLineFromEnd(1);
						}
						if (!endCursor.isNull()) {
							ZLTextWordCursor startCursor = findStart(endCursor, PIXEL_UNIT, textHeight());
							myTextArea.myStartCursor =
								(startCursor != myTextArea.myStartCursor) ?
									startCursor : findStart(myTextArea.myStartCursor, PIXEL_UNIT, textHeight());
						} else {
							myTextArea.myStartCursor = findStart(myTextArea.myStartCursor, PIXEL_UNIT, textHeight());
						}
						break;
					}
					case SCROLL_LINES:
						myTextArea.myStartCursor = findStart(myTextArea.myStartCursor, LINE_UNIT, myOverlappingValue);
						break;
					case SCROLL_PERCENTAGE:
						myTextArea.myStartCursor =
							findStart(myTextArea.myStartCursor, PIXEL_UNIT, textHeight() * myOverlappingValue / 100);
						break;
				}
				myTextArea.myEndCursor = buildInfos(myTextArea.myStartCursor);
				if (pageIsEmpty()) {
					myTextArea.myStartCursor = findStart(myTextArea.myStartCursor, LINE_UNIT, 1);
					myTextArea.myEndCursor = buildInfos(myTextArea.myStartCursor);
				}
			}
			break;
		case START_IS_KNOWN:
			myTextArea.myEndCursor = buildInfos(myTextArea.myStartCursor);
			if (pageIsEmpty()) {
				ZLTextWordCursor startCursor = findLineFromStart(1);
				if (!startCursor.isNull()) {
					myTextArea.myStartCursor = startCursor;
					if (myTextArea.myStartCursor.isEndOfParagraph()) {
						myTextArea.myStartCursor.nextParagraph();
					}
					myTextArea.myEndCursor = buildInfos(myTextArea.myStartCursor);
				}
			}
			break;
		case END_IS_KNOWN:
			myTextArea.myStartCursor = findStart(myTextArea.myEndCursor, PIXEL_UNIT, textHeight());
			myTextArea.myEndCursor = buildInfos(myTextArea.myStartCursor);
			break;
	}
	myPaintState = READY;
	myTextArea.myLineInfoCache.clear();
}

ZLTextWordCursor ZLTextView::findStart(const ZLTextWordCursor &end, SizeUnit unit, int size) {
	ZLTextWordCursor start = end;
	size -= paragraphSize(start, true, unit);
	bool positionChanged = !start.isStartOfParagraph();
	start.moveToParagraphStart();
	while (size > 0) {
		if (positionChanged && start.paragraphCursor().isEndOfSection()) {
			break;
		}
		if (!start.previousParagraph()) {
			break;
		}
		if (!start.paragraphCursor().isEndOfSection()) {
			positionChanged = true;
		}
		size -= paragraphSize(start, false, unit);
	}
	skip(start, unit, -size);

	if (unit != LINE_UNIT) {
		bool sameStart = start == end;
		if (!sameStart && start.isEndOfParagraph() && end.isStartOfParagraph()) {
			ZLTextWordCursor startCopy = start;
			startCopy.nextParagraph();
			sameStart = startCopy == end;
		}
		if (sameStart) {
			start = findStart(end, LINE_UNIT, 1);
		}
	}

	return start;
}

ZLTextWordCursor ZLTextView::buildInfos(const ZLTextWordCursor &start) {
	myTextArea.myLineInfos.clear();

	ZLTextWordCursor cursor = start;
	int textHeight = this->textHeight();
	int counter = 0;
	do {
		ZLTextWordCursor paragraphEnd = cursor;
		paragraphEnd.moveToParagraphEnd();
		ZLTextWordCursor paragraphStart = cursor;
		paragraphStart.moveToParagraphStart();

		ZLTextArea::Style style(textArea(), baseStyle());
		style.applyControls(paragraphStart, cursor);
		ZLTextLineInfoPtr infoPtr = new ZLTextLineInfo(cursor, style.textStyle(), style.bidiLevel());

		while (!infoPtr->End.isEndOfParagraph()) {
			infoPtr = myTextArea.processTextLine(style, infoPtr->End, paragraphEnd);
			textHeight -= infoPtr->Height + infoPtr->Descent;
			if ((textHeight < 0) && (counter > 0)) {
				break;
			}
			textHeight -= infoPtr->VSpaceAfter;
			cursor = infoPtr->End;
			myTextArea.myLineInfos.push_back(infoPtr);
			if (textHeight < 0) {
				break;
			}
			++counter;
		}
	} while (cursor.isEndOfParagraph() && cursor.nextParagraph() && !cursor.paragraphCursor().isEndOfSection() && (textHeight >= 0));

	return cursor;
}

int ZLTextView::paragraphSize(const ZLTextWordCursor &cursor, bool beforeCurrentPosition, SizeUnit unit) {
	ZLTextWordCursor word = cursor;
	word.moveToParagraphStart();
	ZLTextWordCursor end = cursor;
	if (!beforeCurrentPosition) {
		end.moveToParagraphEnd();
	}
	
	int size = 0;

	ZLTextArea::Style style(textArea(), baseStyle());
	while (!word.equalElementIndex(end)) {
		const ZLTextLineInfoPtr info = myTextArea.processTextLine(style, word, end);
		word = info->End;
		size += infoSize(*info, unit);
	}

	return size;
}

void ZLTextView::skip(ZLTextWordCursor &cursor, SizeUnit unit, int size) {
	ZLTextWordCursor paragraphStart = cursor;
	paragraphStart.moveToParagraphStart();
	ZLTextWordCursor paragraphEnd = cursor;
	paragraphEnd.moveToParagraphEnd();

	ZLTextArea::Style style(textArea(), baseStyle());
	style.applyControls(paragraphStart, cursor);

	while (!cursor.isEndOfParagraph() && (size > 0)) {
		const ZLTextLineInfoPtr info = myTextArea.processTextLine(style, cursor, paragraphEnd);
		cursor = info->End;
		size -= infoSize(*info, unit);
	}
}
