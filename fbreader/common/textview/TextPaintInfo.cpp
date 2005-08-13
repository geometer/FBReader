/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <algorithm>

#include "TextView.h"

void TextView::rebuildPaintInfo(bool strong) {
	if (myPaintState == NOTHING_TO_PAINT) {
		return;
	}

	myLineInfos.clear();
	if (!myStartCursor.isNull()) {
		if (strong) {
			myStartCursor.rebuild();
		}
		myEndCursor = 0;
		myPaintState = START_IS_KNOWN;
	} else {
		if (strong) {
			myEndCursor.rebuild();
		}
		myStartCursor = 0;
		myPaintState = END_IS_KNOWN;
	}
}

void TextView::setStartCursor(ParagraphCursor *cursor) {
	myStartCursor = cursor;
	myEndCursor = 0;
	myLineInfos.clear();
	myPaintState = myStartCursor.isNull() ? NOTHING_TO_PAINT : START_IS_KNOWN;
}

void TextView::moveStartCursor(int paragraphNumber, int wordNumber, int charNumber) {
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

void TextView::moveStartCursor(int paragraphNumber, bool start) {
	if (myPaintState == NOTHING_TO_PAINT) {
		return;
	}

	if (myStartCursor.isNull()) {
		myStartCursor = myEndCursor;
	}
	myStartCursor.moveToParagraph(paragraphNumber);
	if (start) {
		myStartCursor.moveToParagraphStart();
	} else {
		myStartCursor.moveToParagraphEnd();
	}
	myEndCursor = 0;
	myLineInfos.clear();
	myPaintState = START_IS_KNOWN;
}

void TextView::moveEndCursor(int paragraphNumber, int wordNumber, int charNumber) {
	if (myPaintState == NOTHING_TO_PAINT) {
		return;
	}

	if (myEndCursor.isNull()) {
		myEndCursor = myStartCursor;
	}
	myEndCursor.moveToParagraph(paragraphNumber);
	myEndCursor.moveTo(wordNumber, charNumber);
	myStartCursor = 0;
	myLineInfos.clear();
	myPaintState = END_IS_KNOWN;
}

void TextView::moveEndCursor(int paragraphNumber, bool start) {
	if (myPaintState == NOTHING_TO_PAINT) {
		return;
	}

	if (myEndCursor.isNull()) {
		myEndCursor = myStartCursor;
	}
	myEndCursor.moveToParagraph(paragraphNumber);
	if (start) {
		myEndCursor.moveToParagraphStart();
	} else {
		myEndCursor.moveToParagraphEnd();
	}
	myStartCursor = 0;
	myLineInfos.clear();
	myPaintState = END_IS_KNOWN;
}

WordCursor TextView::findLineFromStart(unsigned int overlappingValue) const {
	if (myLineInfos.empty() || (overlappingValue == 0)) {
		return WordCursor();
	}

	std::vector<LineInfo>::const_iterator it;
	for (it = myLineInfos.begin(); it != myLineInfos.end(); it++) {
		if (it->IsVisible) {
			overlappingValue--;
			if (overlappingValue == 0) {
				break;
			}
		}
	}
	return (it != myLineInfos.end()) ? it->End : myLineInfos.back().End;
}

WordCursor TextView::findLineFromEnd(unsigned int overlappingValue) const {
	if (myLineInfos.empty() || (overlappingValue == 0)) {
		return WordCursor();
	}

	std::vector<LineInfo>::const_iterator it;
	for (it = myLineInfos.end() - 1; it != myLineInfos.begin(); it--) {
		if (it->IsVisible) {
			overlappingValue--;
			if (overlappingValue == 0) {
				break;
			}
		}
	}
	return it->Start;
}

WordCursor TextView::findPercentFromStart(unsigned int percent) const {
	if (myLineInfos.empty()) {
		return WordCursor();
	}

	int height = myStyle.textAreaHeight() * percent / 100;
	if (height == 0) {
		return myLineInfos.front().Start;
	}
	std::vector<LineInfo>::const_iterator it;
	for (it = myLineInfos.begin(); it != myLineInfos.end(); it++) {
		height -= it->Height;
		if (height <= 0) {
			break;
		}
	}
	return (it != myLineInfos.end()) ? it->End : myLineInfos.back().End;
}

void TextView::preparePaintInfo() {
	context().setLeftMargin(TextStyle::LeftMarginOption.value());
	context().setRightMargin(TextStyle::RightMarginOption.value());
	context().setTopMargin(TextStyle::TopMarginOption.value());
	context().setBottomMargin(TextStyle::BottomMarginOption.value());

	int newWidth = context().width();
	int newHeight = context().height();
	if ((newWidth != myOldWidth) || (newHeight != myOldHeight)) {
		myOldWidth = newWidth;
		myOldHeight = newHeight;
		rebuildPaintInfo(false);
	}

	if ((myPaintState == NOTHING_TO_PAINT) || (myPaintState == READY)) {
		return;
	}

	for (std::vector<LineInfo>::const_iterator it = myLineInfos.begin(); it != myLineInfos.end(); it++) {
		myLineInfoCache.insert(std::pair<WordCursor,LineInfo>(it->Start, *it));
	}

	switch (myPaintState) {
		default:
			break;
		case TO_SCROLL_FORWARD:
			if (!myEndCursor.paragraphCursor().isLast() || !myEndCursor.isEndOfParagraph()) {
				WordCursor startCursor;
				switch (myOverlappingType) {
					case NO_OVERLAPPING:
						break;
					case NUMBER_OF_OVERLAPPED_LINES:
						startCursor = findLineFromEnd(myOverlappingValue);
						break;
					case NUMBER_OF_SCROLLED_LINES:
						startCursor = findLineFromStart(myOverlappingValue);
						break;
					case PERCENT_OF_SCROLLED:
						startCursor = findPercentFromStart(myOverlappingValue);
						break;
				}
      
				if (!startCursor.isNull()) {
					WordCursor endCursor = buildInfos(startCursor);
					if (endCursor != myEndCursor) {
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
				WordCursor endCursor;
				switch (myOverlappingType) {
					case NO_OVERLAPPING:
						break;
					case NUMBER_OF_OVERLAPPED_LINES:
						endCursor = findLineFromStart(myOverlappingValue);
						break;
					case NUMBER_OF_SCROLLED_LINES:
						endCursor = findLineFromEnd(myOverlappingValue);
						break;
					case PERCENT_OF_SCROLLED:
						endCursor = findPercentFromStart(100 - myOverlappingValue);
						break;
				}
				if (!endCursor.isNull()) {
					WordCursor startCursor = findStart(endCursor);
					myStartCursor = (startCursor != myStartCursor) ? startCursor : findStart(myStartCursor);
				} else {
					myStartCursor = findStart(myStartCursor);
				}
				myEndCursor = buildInfos(myStartCursor);
			}
			break;
		case START_IS_KNOWN:
			myEndCursor = buildInfos(myStartCursor);
			break;
		case END_IS_KNOWN:
			myStartCursor = findStart(myEndCursor);
			myEndCursor = buildInfos(myStartCursor);
			break;
	}
	myPaintState = READY;
	myLineInfoCache.clear();
}

WordCursor TextView::findStart(const WordCursor &end) {
	WordCursor start = end;
	int height = paragraphHeight(start, true);
	bool positionChanged = !start.isStartOfParagraph();
	start.moveToParagraphStart();
	const int textAreaHeight = myStyle.textAreaHeight();
	while (height < textAreaHeight) {
		if (positionChanged && start.paragraphCursor().isEndOfSection()) {
			break;
		}
		if (!start.previousParagraph()) {
			break;
		}
		if (!start.paragraphCursor().isEndOfSection()) {
			positionChanged = true;
		}
		height += paragraphHeight(start, false);
	}
	skip(start, height - textAreaHeight);
	return start;
}

WordCursor TextView::buildInfos(const WordCursor &start) {
	myLineInfos.clear();

	WordCursor end = start;
	int textAreaHeight = myStyle.textAreaHeight();
	do {
		LineInfo info(end, myStyle.style());
		WordCursor paragraphEnd = end;
		paragraphEnd.moveToParagraphEnd();
		WordCursor start = end;
		start.moveToParagraphStart();

		myStyle.reset();
		myStyle.applyControls(start, info.Start);

		while (!info.End.isEndOfParagraph()) {
			info = processTextLine(info.End, paragraphEnd);
			textAreaHeight -= info.Height;
			if (textAreaHeight < 0) {
				break;
			}
			end = info.End;
			myLineInfos.push_back(info);
		}
	} while (end.isEndOfParagraph() && end.nextParagraph() && !end.paragraphCursor().isEndOfSection());

	return end;
}
