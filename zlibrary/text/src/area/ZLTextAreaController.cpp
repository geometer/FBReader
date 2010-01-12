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

#include "ZLTextAreaController.h"
#include "ZLTextArea.h"
#include "ZLTextAreaStyle.h"
#include "ZLTextLineInfo.h"

ZLTextAreaController::ZLTextAreaController(ZLPaintContext &context, const ZLTextArea::Properties &properties) : myArea(context, properties), myPaintState(NOTHING_TO_PAINT) {
}

ZLTextAreaController::~ZLTextAreaController() {
}

void ZLTextAreaController::setModel(shared_ptr<ZLTextModel> model) {
	myArea.setModel(model);

	myPaintState = myArea.isEmpty() ? NOTHING_TO_PAINT : START_IS_KNOWN;
}

void ZLTextAreaController::clear() {
	myArea.clear();

	myPaintState = NOTHING_TO_PAINT;
	ZLTextParagraphCursorCache::clear();
}

ZLTextWordCursor ZLTextAreaController::findStart(const ZLTextWordCursor &end, SizeUnit unit, int size) {
	ZLTextWordCursor start = end;
	size -= paragraphHeight(start, true, unit);
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
		size -= paragraphHeight(start, false, unit);
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

ZLTextWordCursor ZLTextAreaController::buildInfos(const ZLTextWordCursor &start) {
	myArea.myLineInfos.clear();

	ZLTextWordCursor cursor = start;
	int textHeight = myArea.height();
	int counter = 0;
	do {
		ZLTextWordCursor paragraphEnd = cursor;
		paragraphEnd.moveToParagraphEnd();
		ZLTextWordCursor paragraphStart = cursor;
		paragraphStart.moveToParagraphStart();

		ZLTextArea::Style style(myArea, myArea.myProperties.baseStyle());
		style.applyControls(paragraphStart, cursor);
		ZLTextLineInfoPtr info = new ZLTextLineInfo(cursor, style.textStyle(), style.bidiLevel());

		while (!info->End.isEndOfParagraph()) {
			info = myArea.processTextLine(style, info->End, paragraphEnd);
			textHeight -= info->Height + info->Descent;
			if ((textHeight < 0) && (counter > 0)) {
				break;
			}
			textHeight -= info->VSpaceAfter;
			cursor = info->End;
			myArea.myLineInfos.push_back(info);
			if (textHeight < 0) {
				break;
			}
			++counter;
		}
	} while (cursor.isEndOfParagraph() && cursor.nextParagraph() && !cursor.paragraphCursor().isEndOfSection() && (textHeight >= 0));

	return cursor;
}

void ZLTextAreaController::skip(ZLTextWordCursor &cursor, SizeUnit unit, int size) {
	ZLTextWordCursor paragraphStart = cursor;
	paragraphStart.moveToParagraphStart();
	ZLTextWordCursor paragraphEnd = cursor;
	paragraphEnd.moveToParagraphEnd();

	ZLTextArea::Style style(myArea, myArea.myProperties.baseStyle());
	style.applyControls(paragraphStart, cursor);

	while (!cursor.isEndOfParagraph() && (size > 0)) {
		const ZLTextLineInfoPtr info = 
			myArea.processTextLine(style, cursor, paragraphEnd);
		cursor = info->End;
		size -= infoHeight(*info, unit);
	}
}

bool ZLTextAreaController::visiblePageIsEmpty() const {
	for (std::vector<ZLTextLineInfoPtr>::const_iterator it = myArea.myLineInfos.begin(); it != myArea.myLineInfos.end(); ++it) {
		if ((*it)->IsVisible) {
			return false;
		}
	}
	return true;
}

int ZLTextAreaController::infoHeight(const ZLTextLineInfo &info, SizeUnit unit) {
	return
		unit == PIXEL_UNIT ?
			info.Height + info.Descent + info.VSpaceAfter :
			info.IsVisible ? 1 : 0;
}

int ZLTextAreaController::paragraphHeight(const ZLTextWordCursor &cursor, bool beforeCurrentPosition, SizeUnit unit) {
	ZLTextWordCursor word = cursor;
	word.moveToParagraphStart();
	ZLTextWordCursor end = cursor;
	if (!beforeCurrentPosition) {
		end.moveToParagraphEnd();
	}
	
	int size = 0;

	ZLTextArea::Style style(myArea, myArea.myProperties.baseStyle());
	while (!word.equalElementIndex(end)) {
		const ZLTextLineInfoPtr info = myArea.processTextLine(style, word, end);
		word = info->End;
		size += infoHeight(*info, unit);
	}

	return size;
}

ZLTextWordCursor ZLTextAreaController::findLineFromStart(unsigned int overlappingValue) const {
	if (myArea.myLineInfos.empty() || (overlappingValue == 0)) {
		return ZLTextWordCursor();
	}

	std::vector<ZLTextLineInfoPtr>::const_iterator it;
	for (it = myArea.myLineInfos.begin(); it != myArea.myLineInfos.end(); ++it) {
		if ((*it)->IsVisible) {
			--overlappingValue;
			if (overlappingValue == 0) {
				break;
			}
		}
	}
	return it != myArea.myLineInfos.end() ? (*it)->End : myArea.myLineInfos.back()->End;
}

ZLTextWordCursor ZLTextAreaController::findLineFromEnd(unsigned int overlappingValue) const {
	if (myArea.myLineInfos.empty() || (overlappingValue == 0)) {
		return ZLTextWordCursor();
	}

	std::vector<ZLTextLineInfoPtr>::const_iterator it;
	for (it = myArea.myLineInfos.end() - 1; it != myArea.myLineInfos.begin(); --it) {
		if ((*it)->IsVisible) {
			--overlappingValue;
			if (overlappingValue == 0) {
				break;
			}
		}
	}
	return (*it)->Start;
}

ZLTextWordCursor ZLTextAreaController::findPercentFromStart(unsigned int percent) const {
	if (myArea.myLineInfos.empty()) {
		return ZLTextWordCursor();
	}

	int height = myArea.height() * percent / 100;
	bool visibleLineOccured = false;
	std::vector<ZLTextLineInfoPtr>::const_iterator it;
	for (it = myArea.myLineInfos.begin(); it != myArea.myLineInfos.end(); ++it) {
		const ZLTextLineInfo &info = **it;
		if (info.IsVisible) {
			visibleLineOccured = true;
		}
		height -= info.Height + info.Descent + info.VSpaceAfter;
		if (visibleLineOccured && (height <= 0)) {
			break;
		}
	}
	return it != myArea.myLineInfos.end() ? (*it)->End : myArea.myLineInfos.back()->End;
}

void ZLTextAreaController::rebuildPaintInfo(bool strong) {
	if (myPaintState == NOTHING_TO_PAINT) {
		return;
	}

	if ((myPaintState == TO_SCROLL_FORWARD) || (myPaintState == TO_SCROLL_BACKWARD)) {
		preparePaintInfo();
	}

	myArea.myLineInfos.clear();
	if (strong) {
		ZLTextParagraphCursorCache::clear();
		myArea.myLineInfoCache.clear();
	}

	if (!myArea.myStartCursor.isNull()) {
		if (strong) {
			myArea.myStartCursor.rebuild();
		}
		myArea.myEndCursor = 0;
		myPaintState = START_IS_KNOWN;
	} else if (!myArea.myEndCursor.isNull()) {
		if (strong) {
			myArea.myEndCursor.rebuild();
		}
		myPaintState = END_IS_KNOWN;
	}
}

bool ZLTextAreaController::preparePaintInfo() {
	if ((myPaintState == NOTHING_TO_PAINT) || (myPaintState == READY)) {
		return false;
	}

	myArea.myLineInfoCache.insert(myArea.myLineInfos.begin(), myArea.myLineInfos.end());

	switch (myPaintState) {
		default:
			break;
		case TO_SCROLL_FORWARD:
			if (!myArea.myEndCursor.paragraphCursor().isLast() ||
					!myArea.myEndCursor.isEndOfParagraph()) {
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
			
				if (!startCursor.isNull() && (startCursor == myArea.myStartCursor)) {
					startCursor = findLineFromStart(1);
				}

				if (!startCursor.isNull()) {
					ZLTextWordCursor endCursor = buildInfos(startCursor);
					if (!visiblePageIsEmpty() && ((myScrollingMode != KEEP_LINES) || (endCursor != myArea.myEndCursor))) {
						myArea.myStartCursor = startCursor;
						myArea.myEndCursor = endCursor;
						break;
					}
				}
				myArea.myStartCursor = myArea.myEndCursor;
				myArea.myEndCursor = buildInfos(myArea.myStartCursor);
			}
			break;
		case TO_SCROLL_BACKWARD:
			if (!myArea.myStartCursor.paragraphCursor().isFirst() || !myArea.myStartCursor.isStartOfParagraph()) {
				switch (myScrollingMode) {
					case NO_OVERLAPPING:
						myArea.myStartCursor = findStart(myArea.myStartCursor, PIXEL_UNIT, myArea.height());
						break;
					case KEEP_LINES:
					{
						ZLTextWordCursor endCursor = findLineFromStart(myOverlappingValue);
						if (!endCursor.isNull() && (endCursor == myArea.myEndCursor)) {
							endCursor = findLineFromEnd(1);
						}
						if (!endCursor.isNull()) {
							ZLTextWordCursor startCursor = findStart(endCursor, PIXEL_UNIT, myArea.height());
							myArea.myStartCursor =
								(startCursor != myArea.myStartCursor) ?
									startCursor : findStart(myArea.myStartCursor, PIXEL_UNIT, myArea.height());
						} else {
							myArea.myStartCursor = findStart(myArea.myStartCursor, PIXEL_UNIT, myArea.height());
						}
						break;
					}
					case SCROLL_LINES:
						myArea.myStartCursor = findStart(myArea.myStartCursor, LINE_UNIT, myOverlappingValue);
						break;
					case SCROLL_PERCENTAGE:
						myArea.myStartCursor =
							findStart(myArea.myStartCursor, PIXEL_UNIT, myArea.height() * myOverlappingValue / 100);
						break;
				}
				myArea.myEndCursor = buildInfos(myArea.myStartCursor);
				if (visiblePageIsEmpty()) {
					myArea.myStartCursor = findStart(myArea.myStartCursor, LINE_UNIT, 1);
					myArea.myEndCursor = buildInfos(myArea.myStartCursor);
				}
			}
			break;
		case START_IS_KNOWN:
			myArea.myEndCursor = buildInfos(myArea.myStartCursor);
			if (visiblePageIsEmpty()) {
				ZLTextWordCursor startCursor = findLineFromStart(1);
				if (!startCursor.isNull()) {
					myArea.myStartCursor = startCursor;
					if (myArea.myStartCursor.isEndOfParagraph()) {
						myArea.myStartCursor.nextParagraph();
					}
					myArea.myEndCursor = buildInfos(myArea.myStartCursor);
				}
			}
			break;
		case END_IS_KNOWN:
			myArea.myStartCursor = findStart(myArea.myEndCursor, PIXEL_UNIT, myArea.height());
			myArea.myEndCursor = buildInfos(myArea.myStartCursor);
			break;
	}
	myPaintState = READY;
	myArea.myLineInfoCache.clear();
	return true;
}

void ZLTextAreaController::moveStartCursor(int paragraphIndex, int elementIndex, int charIndex) {
	if (myPaintState == NOTHING_TO_PAINT) {
		return;
	}

	if (myArea.myStartCursor.isNull()) {
		myArea.myStartCursor = myArea.myEndCursor;
	}
	myArea.myStartCursor.moveToParagraph(paragraphIndex);
	myArea.myStartCursor.moveTo(elementIndex, charIndex);
	myArea.myEndCursor = 0;
	myArea.myLineInfos.clear();
	myPaintState = START_IS_KNOWN;
}

void ZLTextAreaController::moveEndCursor(int paragraphIndex, int elementIndex, int charIndex) {
	if (myPaintState == NOTHING_TO_PAINT) {
		return;
	}

	if (myArea.myEndCursor.isNull()) {
		myArea.myEndCursor = myArea.myStartCursor;
	}
	myArea.myEndCursor.moveToParagraph(paragraphIndex);
	if ((paragraphIndex > 0) && (elementIndex == 0) && (charIndex == 0)) {
		myArea.myEndCursor.previousParagraph();
		myArea.myEndCursor.moveToParagraphEnd();
	} else {
		myArea.myEndCursor.moveTo(elementIndex, charIndex);
	}
	myArea.myStartCursor = 0;
	myArea.myLineInfos.clear();
	myPaintState = END_IS_KNOWN;
}

void ZLTextAreaController::scrollPage(bool forward, ScrollingMode mode, unsigned int value) {
	if (myPaintState != READY) {
		return;
	}
	myPaintState = forward ? TO_SCROLL_FORWARD : TO_SCROLL_BACKWARD;
	myScrollingMode = mode;
	myOverlappingValue = value;
}
