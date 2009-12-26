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

#include "ZLTextArea.h"
#include "ZLTextAreaStyle.h"
#include "ZLTextLineInfo.h"

ZLTextWordCursor ZLTextArea::findStart(const ZLTextWordCursor &end, SizeUnit unit, int size) {
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

ZLTextWordCursor ZLTextArea::buildInfos(const ZLTextWordCursor &start) {
	myLineInfos.clear();

	ZLTextWordCursor cursor = start;
	int textHeight = height();
	int counter = 0;
	do {
		ZLTextWordCursor paragraphEnd = cursor;
		paragraphEnd.moveToParagraphEnd();
		ZLTextWordCursor paragraphStart = cursor;
		paragraphStart.moveToParagraphStart();

		Style style(*this, myProperties.baseStyle());
		style.applyControls(paragraphStart, cursor);
		ZLTextLineInfoPtr infoPtr = new ZLTextLineInfo(cursor, style.textStyle(), style.bidiLevel());

		while (!infoPtr->End.isEndOfParagraph()) {
			infoPtr = processTextLine(style, infoPtr->End, paragraphEnd);
			textHeight -= infoPtr->Height + infoPtr->Descent;
			if ((textHeight < 0) && (counter > 0)) {
				break;
			}
			textHeight -= infoPtr->VSpaceAfter;
			cursor = infoPtr->End;
			myLineInfos.push_back(infoPtr);
			if (textHeight < 0) {
				break;
			}
			++counter;
		}
	} while (cursor.isEndOfParagraph() && cursor.nextParagraph() && !cursor.paragraphCursor().isEndOfSection() && (textHeight >= 0));

	return cursor;
}

void ZLTextArea::skip(ZLTextWordCursor &cursor, SizeUnit unit, int size) {
	ZLTextWordCursor paragraphStart = cursor;
	paragraphStart.moveToParagraphStart();
	ZLTextWordCursor paragraphEnd = cursor;
	paragraphEnd.moveToParagraphEnd();

	Style style(*this, myProperties.baseStyle());
	style.applyControls(paragraphStart, cursor);

	while (!cursor.isEndOfParagraph() && (size > 0)) {
		const ZLTextLineInfoPtr info = processTextLine(style, cursor, paragraphEnd);
		cursor = info->End;
		size -= infoHeight(*info, unit);
	}
}

bool ZLTextArea::visiblePageIsEmpty() const {
	for (std::vector<ZLTextLineInfoPtr>::const_iterator it = myLineInfos.begin(); it != myLineInfos.end(); ++it) {
		if ((*it)->IsVisible) {
			return false;
		}
	}
	return true;
}

int ZLTextArea::infoHeight(const ZLTextLineInfo &info, SizeUnit unit) {
	return
		unit == PIXEL_UNIT ?
			info.Height + info.Descent + info.VSpaceAfter :
			info.IsVisible ? 1 : 0;
}

int ZLTextArea::paragraphHeight(const ZLTextWordCursor &cursor, bool beforeCurrentPosition, SizeUnit unit) {
	ZLTextWordCursor word = cursor;
	word.moveToParagraphStart();
	ZLTextWordCursor end = cursor;
	if (!beforeCurrentPosition) {
		end.moveToParagraphEnd();
	}
	
	int size = 0;

	Style style(*this, myProperties.baseStyle());
	while (!word.equalElementIndex(end)) {
		const ZLTextLineInfoPtr info = processTextLine(style, word, end);
		word = info->End;
		size += infoHeight(*info, unit);
	}

	return size;
}
