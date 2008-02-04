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

#include <algorithm>

#include "ZLTextView.h"
#include "ZLTextLineInfo.h"

void ZLTextView::rebuildPaintInfo(bool strong) {
	if (myPaintState == NOTHING_TO_PAINT) {
		return;
	}

	myLineInfos.clear();
	if (strong) {
		ZLTextParagraphCursorCache::clear();
		myLineInfoCache.clear();
	}
	if (!myStartCursor.isNull()) {
		if (strong) {
			myStartCursor.rebuild();
		}
		myEndCursor = 0;
		myPaintState = START_IS_KNOWN;
	} else if (!myEndCursor.isNull()) {
		if (strong) {
			myEndCursor.rebuild();
		}
		myPaintState = END_IS_KNOWN;
	}
}

void ZLTextView::setStartCursor(ZLTextParagraphCursorPtr cursor) {
	myStartCursor = cursor;
	myEndCursor = 0;
	myLineInfos.clear();
	myPaintState = myStartCursor.isNull() ? NOTHING_TO_PAINT : START_IS_KNOWN;
}

void ZLTextView::moveStartCursor(int paragraphNumber, int wordNumber, int charNumber) {
	if (myPaintState == NOTHING_TO_PAINT) {
		return;
	}

	if (myStartCursor.isNull()) {
		myStartCursor = myEndCursor;
	}
	myStartCursor.moveToParagraph(paragraphNumber);
	myStartCursor.moveTo(wordNumber, charNumber);
	myEndCursor = 0;
	myLineInfos.clear();
	myPaintState = START_IS_KNOWN;
}

void ZLTextView::moveEndCursor(int paragraphNumber, int wordNumber, int charNumber) {
	if (myPaintState == NOTHING_TO_PAINT) {
		return;
	}

	if (myEndCursor.isNull()) {
		myEndCursor = myStartCursor;
	}
	myEndCursor.moveToParagraph(paragraphNumber);
	if ((paragraphNumber > 0) && (wordNumber == 0) && (charNumber == 0)) {
		myEndCursor.previousParagraph();
		myEndCursor.moveToParagraphEnd();
	} else {
		myEndCursor.moveTo(wordNumber, charNumber);
	}
	myStartCursor = 0;
	myLineInfos.clear();
	myPaintState = END_IS_KNOWN;
}

bool ZLTextView::pageIsEmpty() const {
	for (std::vector<ZLTextLineInfoPtr>::const_iterator it = myLineInfos.begin(); it != myLineInfos.end(); ++it) {
		if ((*it)->IsVisible) {
			return false;
		}
	}
	return true;
}

ZLTextWordCursor ZLTextView::findLineFromStart(unsigned int overlappingValue) const {
	if (myLineInfos.empty() || (overlappingValue == 0)) {
		return ZLTextWordCursor();
	}

	std::vector<ZLTextLineInfoPtr>::const_iterator it;
	for (it = myLineInfos.begin(); it != myLineInfos.end(); ++it) {
		if ((*it)->IsVisible) {
			--overlappingValue;
			if (overlappingValue == 0) {
				break;
			}
		}
	}
	return (it != myLineInfos.end()) ? (*it)->End : (*myLineInfos.back()).End;
}

ZLTextWordCursor ZLTextView::findLineFromEnd(unsigned int overlappingValue) const {
	if (myLineInfos.empty() || (overlappingValue == 0)) {
		return ZLTextWordCursor();
	}

	std::vector<ZLTextLineInfoPtr>::const_iterator it;
	for (it = myLineInfos.end() - 1; it != myLineInfos.begin(); --it) {
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
	if (myLineInfos.empty()) {
		return ZLTextWordCursor();
	}

	int height = textAreaHeight() * percent / 100;
	bool visibleLineOccured = false;
	std::vector<ZLTextLineInfoPtr>::const_iterator it;
	for (it = myLineInfos.begin(); it != myLineInfos.end(); ++it) {
		const ZLTextLineInfo &info = **it;
		if (info.IsVisible) {
			visibleLineOccured = true;
		}
		height -= info.Height + info.Descent + info.VSpaceAfter;
		if (visibleLineOccured && (height <= 0)) {
			break;
		}
	}
	return (it != myLineInfos.end()) ? (*it)->End : (*myLineInfos.back()).End;
}

void ZLTextView::preparePaintInfo() {
	int newWidth = viewWidth();
	int newHeight = textAreaHeight();
	if ((newWidth != myOldWidth) || (newHeight != myOldHeight)) {
		myOldWidth = newWidth;
		myOldHeight = newHeight;
		rebuildPaintInfo(false);
	}

	if ((myPaintState == NOTHING_TO_PAINT) || (myPaintState == READY)) {
		return;
	}

	myLineInfoCache.insert(myLineInfos.begin(), myLineInfos.end());

	switch (myPaintState) {
		default:
			break;
		case TO_SCROLL_FORWARD:
			if (!myEndCursor.paragraphCursor().isLast() || !myEndCursor.isEndOfParagraph()) {
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
			
				if (!startCursor.isNull() && (startCursor == myStartCursor)) {
					startCursor = findLineFromStart(1);
				}

				if (!startCursor.isNull()) {
					ZLTextWordCursor endCursor = buildInfos(startCursor);
					if (!pageIsEmpty() && ((myScrollingMode != KEEP_LINES) || (endCursor != myEndCursor))) {
						myStartCursor = startCursor;
						myEndCursor = endCursor;
						break;
					}
				}
				myStartCursor = myEndCursor;
				myEndCursor = buildInfos(myStartCursor);
			}
			break;
		case TO_SCROLL_BACKWARD:
			if (!myStartCursor.paragraphCursor().isFirst() || !myStartCursor.isStartOfParagraph()) {
				switch (myScrollingMode) {
					case NO_OVERLAPPING:
						myStartCursor = findStart(myStartCursor, PIXEL_UNIT, textAreaHeight());
						break;
					case KEEP_LINES:
					{
						ZLTextWordCursor endCursor = findLineFromStart(myOverlappingValue);
						if (!endCursor.isNull() && (endCursor == myEndCursor)) {
							endCursor = findLineFromEnd(1);
						}
						if (!endCursor.isNull()) {
							ZLTextWordCursor startCursor = findStart(endCursor, PIXEL_UNIT, textAreaHeight());
							myStartCursor =
								(startCursor != myStartCursor) ?
									startCursor : findStart(myStartCursor, PIXEL_UNIT, textAreaHeight());
						} else {
							myStartCursor = findStart(myStartCursor, PIXEL_UNIT, textAreaHeight());
						}
						break;
					}
					case SCROLL_LINES:
						myStartCursor = findStart(myStartCursor, LINE_UNIT, myOverlappingValue);
						break;
					case SCROLL_PERCENTAGE:
						myStartCursor =
							findStart(myStartCursor, PIXEL_UNIT, textAreaHeight() * myOverlappingValue / 100);
						break;
				}
				myEndCursor = buildInfos(myStartCursor);
				if (pageIsEmpty()) {
					myStartCursor = findStart(myStartCursor, LINE_UNIT, 1);
					myEndCursor = buildInfos(myStartCursor);
				}
			}
			break;
		case START_IS_KNOWN:
			myEndCursor = buildInfos(myStartCursor);
			break;
		case END_IS_KNOWN:
			myStartCursor = findStart(myEndCursor, PIXEL_UNIT, textAreaHeight());
			myEndCursor = buildInfos(myStartCursor);
			break;
	}
	myPaintState = READY;
	myLineInfoCache.clear();
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
	myLineInfos.clear();

	ZLTextWordCursor cursor = start;
	int textAreaHeight = this->textAreaHeight();
	int counter = 0;
	do {
		ZLTextWordCursor paragraphEnd = cursor;
		paragraphEnd.moveToParagraphEnd();
		ZLTextWordCursor paragraphStart = cursor;
		paragraphStart.moveToParagraphStart();

		myStyle.reset();
		myStyle.applyControls(paragraphStart, cursor);
		ZLTextLineInfoPtr infoPtr = new ZLTextLineInfo(cursor, myStyle.textStyle());

		while (!infoPtr->End.isEndOfParagraph()) {
			infoPtr = processTextLine(infoPtr->End, paragraphEnd);
			textAreaHeight -= infoPtr->Height + infoPtr->Descent;
			if ((textAreaHeight < 0) && (counter > 0)) {
				break;
			}
			textAreaHeight -= infoPtr->VSpaceAfter;
			cursor = infoPtr->End;
			myLineInfos.push_back(infoPtr);
			if (textAreaHeight < 0) {
				break;
			}
			++counter;
		}
	} while (cursor.isEndOfParagraph() && cursor.nextParagraph() && !cursor.paragraphCursor().isEndOfSection() && (textAreaHeight >= 0));

	return cursor;
}

int ZLTextView::paragraphSize(const ZLTextWordCursor &cursor, bool beforeCurrentPosition, SizeUnit unit) {
	ZLTextWordCursor word = cursor;
	word.moveToParagraphStart();
	ZLTextWordCursor end = cursor;
	if (!beforeCurrentPosition) {
		end.moveToParagraphEnd();
	}
	
	myStyle.reset();

	int size = 0;

	while (!word.equalWordNumber(end)) {
		const ZLTextLineInfoPtr info = processTextLine(word, end);
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

	myStyle.reset();
	myStyle.applyControls(paragraphStart, cursor);

	while (!cursor.isEndOfParagraph() && (size > 0)) {
		const ZLTextLineInfoPtr info = processTextLine(cursor, paragraphEnd);
		cursor = info->End;
		size -= infoSize(*info, unit);
	}
}
