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

#include <ZLApplication.h>
#include <ZLTime.h>
#include <ZLUnicodeUtil.h>
#include <ZLibrary.h>

#include "ZLTextSelectionModel.h"
#include "ZLTextArea.h"
#include "ZLTextAreaStyle.h"

ZLTextSelectionModel::ZLTextSelectionModel(ZLTextArea &area) : myArea(area), myIsActive(false), myIsEmpty(true), myTextIsUpToDate(true), myRangeVectorIsUpToDate(true), myIsValid(true) {
}

ZLTextSelectionModel::~ZLTextSelectionModel() {
}

void ZLTextSelectionModel::clearData() const {
	myText.erase();
	myImage.reset();
}

int ZLTextSelectionModel::charIndex(const ZLTextElementRectangle &rectangle, int x) {
	int x1 = x - myArea.hOffset();
	ZLTextArea::Style style(myArea, rectangle.Style);
	style.setTextStyle(rectangle.Style, rectangle.BidiLevel);
	ZLTextWordCursor cursor = myArea.startCursor();
	cursor.moveToParagraph(rectangle.ParagraphIndex);
	const ZLTextWord &word = (const ZLTextWord&)cursor.paragraphCursor()[rectangle.ElementIndex];
	const bool mainDir =
		rectangle.BidiLevel % 2 == (myArea.isRtl() ? 1 : 0);
	const int deltaX = mainDir ? x1 - rectangle.XStart : rectangle.XEnd - x1;
	const int len = rectangle.Length;
	const int start = rectangle.StartCharIndex;
	int diff = deltaX;
	int previousDiff = diff;
	int index;
	for (index = 0; (index < len) && (diff > 0); ++index) {
		previousDiff = diff;
		diff = deltaX - style.wordWidth(word, start, index + 1);
	}
	if (previousDiff + diff < 0) {
		--index;
	}
	return start + index;
}

void ZLTextSelectionModel::setBound(Bound &bound, int x, int y) {
	int x1 = x - myArea.hOffset();
	int y1 = y - myArea.vOffset();

	if (myArea.myTextElementMap.empty()) {
		return;
	}

	ZLTextElementMap::const_iterator it = myArea.myTextElementMap.begin();
	for (; it != myArea.myTextElementMap.end(); ++it) {
		if ((it->YStart > y1) || ((it->YEnd > y1) && (it->XEnd > x1))) {
			break;
		}
	}

	if (it != myArea.myTextElementMap.end()) {
		bound.After.ParagraphIndex = it->ParagraphIndex;
		bound.After.ElementIndex = it->ElementIndex;
		bound.After.Exists = true;
		const bool mainDir =
			it->BidiLevel % 2 == (myArea.isRtl() ? 1 : 0);
		bound.After.CharIndex = mainDir ?
			it->StartCharIndex :
			it->StartCharIndex + it->Length;
		if (ZLTextElementRectangle::RangeChecker(x1, y1)(*it)) {
			bound.Before.ParagraphIndex = bound.After.ParagraphIndex;
			bound.Before.ElementIndex = bound.After.ElementIndex;
			bound.Before.Exists = true;
			if (it->Kind == ZLTextElement::WORD_ELEMENT) {
				bound.After.CharIndex = charIndex(*it, x);
				bound.Before.CharIndex = bound.After.CharIndex;
			}
		} else if (it == myArea.myTextElementMap.begin()) {
			bound.Before.Exists = false;
		} else {
			const ZLTextElementRectangle &previous = *(it - 1);
			bound.Before.ParagraphIndex = previous.ParagraphIndex;
			bound.Before.ElementIndex = previous.ElementIndex;
			bound.Before.CharIndex =
				(previous.BidiLevel % 2 == (myArea.isRtl() ? 1 : 0)) ?
					previous.StartCharIndex + previous.Length :
					previous.StartCharIndex;
			bound.Before.Exists = true;
		}
	} else {
		const ZLTextElementRectangle &back = myArea.myTextElementMap.back();
		bound.Before.ParagraphIndex = back.ParagraphIndex;
		bound.Before.ElementIndex = back.ElementIndex;
		bound.Before.CharIndex = back.StartCharIndex + back.Length;
		bound.Before.Exists = true;
		bound.After.Exists = false;
	}
}

void ZLTextSelectionModel::activate(int x, int y) {
	if (myArea.myTextElementMap.empty()) {
		return;
	}

	myIsActive = true;
	myIsEmpty = false;
	setBound(myFirstBound, x, y);
	mySecondBound = myFirstBound;
	myCursors.clear();
	clearData();
	myTextIsUpToDate = true;
	myRanges.clear();
	myRangeVectorIsUpToDate = true;
}

bool ZLTextSelectionModel::BoundElement::operator == (const ZLTextSelectionModel::BoundElement &element) const {
	return
		(Exists == element.Exists) &&
		(ParagraphIndex == element.ParagraphIndex) &&
		(ElementIndex == element.ElementIndex) &&
		(CharIndex == element.CharIndex);
}

bool ZLTextSelectionModel::BoundElement::operator != (const ZLTextSelectionModel::BoundElement &element) const {
	return !operator == (element);
}

ZLTextSelectionModel::ExtensionResult ZLTextSelectionModel::extendTo(int x, int y) {
	if (!myIsActive || myArea.myTextElementMap.empty()) {
		return BOUND_NOT_CHANGED;
	}

	Range oldRange = internalRange();
	setBound(mySecondBound, x, y);
	Range newRange = internalRange();
	myStoredX = x;
	myStoredY = y;

	ExtensionResult result = BOUND_NOT_CHANGED;
	if ((oldRange.first != newRange.first) || (oldRange.second != newRange.second)) {
		myTextIsUpToDate = false;
		clearData();
		myRangeVectorIsUpToDate = false;
		myRanges.clear();
		copySelectionToClipboard(ZLDialogManager::CLIPBOARD_SELECTION);
		result = BOUND_CHANGED;
	}

	if (!mySecondBound.Before.Exists) {
		return BOUND_OVER_BEFORE;
	} else if (!mySecondBound.After.Exists) {
		return BOUND_OVER_AFTER;
	}

	return result;
}

void ZLTextSelectionModel::deactivate() {
	myIsActive = false;
}

void ZLTextSelectionModel::clear() {
	myIsEmpty = true;
	myIsActive = false;
	myCursors.clear();
	clearData();
	myTextIsUpToDate = true;
	myRanges.clear();
	myRangeVectorIsUpToDate = true;
	myIsValid = true;
}

const std::vector<ZLTextSelectionModel::Range> &ZLTextSelectionModel::ranges() const {
	if (!myRangeVectorIsUpToDate && !isEmpty()) {
		Range r = internalRange();

		ZLTextWordCursor cursor = myArea.startCursor();
		cursor.moveToParagraph(r.first.ParagraphIndex);
		cursor.moveToParagraphStart();
		int startLevel = 0;
		for (int i = r.first.ElementIndex; i > 0; --i) {
			switch (cursor.element().kind()) {
				case ZLTextElement::START_REVERSED_SEQUENCE_ELEMENT:
					++startLevel;
					break;
				case ZLTextElement::END_REVERSED_SEQUENCE_ELEMENT:
					--startLevel;
					break;
				default:
					break;
			}
			cursor.nextWord();
		}

		cursor.moveToParagraph(r.second.ParagraphIndex);
		cursor.moveToParagraphEnd();
		int endLevel = 0;
		for (int i = cursor.elementIndex() - r.second.ElementIndex; i > 0; --i) {
			cursor.previousWord();
			switch (cursor.element().kind()) {
				case ZLTextElement::START_REVERSED_SEQUENCE_ELEMENT:
					--endLevel;
					break;
				case ZLTextElement::END_REVERSED_SEQUENCE_ELEMENT:
					++endLevel;
					break;
				default:
					break;
			}
		}

		if ((startLevel == 0) && (endLevel == 0)) {
			myRanges.push_back(r);
		} else if (r.first.ParagraphIndex == r.second.ParagraphIndex) {
			BoundElement leftBound = r.first;
			BoundElement rightBound;
			rightBound.Exists = true;
			rightBound.ParagraphIndex = leftBound.ParagraphIndex;
			rightBound.CharIndex = 0;
			cursor.moveTo(r.first.ElementIndex, 0);
			for (int i = r.first.ElementIndex; i < r.second.ElementIndex; ++i) {
				ZLTextElement::Kind kind = cursor.element().kind();
				if ((kind == ZLTextElement::START_REVERSED_SEQUENCE_ELEMENT) ||
						(kind == ZLTextElement::END_REVERSED_SEQUENCE_ELEMENT)) {
					rightBound.ElementIndex = i;
					myRanges.push_back(Range(leftBound, rightBound));
					leftBound = rightBound;
				}
				cursor.nextWord();
			}
			myRanges.push_back(Range(leftBound, r.second));
		} else {
			BoundElement leftBound = r.first;
			if (startLevel > 0) {
				BoundElement rightBound;
				rightBound.Exists = true;
				rightBound.ParagraphIndex = leftBound.ParagraphIndex;
				rightBound.ElementIndex = leftBound.ElementIndex;
				rightBound.CharIndex = 0;
				cursor.moveToParagraph(r.first.ParagraphIndex);
				cursor.moveTo(r.first.ElementIndex, 0);
				while (startLevel > 0) {
					switch(cursor.element().kind()) {
						case ZLTextElement::START_REVERSED_SEQUENCE_ELEMENT:
							++startLevel;
							rightBound.ElementIndex = cursor.elementIndex();
							myRanges.push_back(Range(leftBound, rightBound));
							leftBound = rightBound;
							break;
						case ZLTextElement::END_REVERSED_SEQUENCE_ELEMENT:
							--startLevel;
							rightBound.ElementIndex = cursor.elementIndex();
							myRanges.push_back(Range(leftBound, rightBound));
							leftBound = rightBound;
							break;
						default:
							break;
					}
					cursor.nextWord();
				}
			}

			BoundElement rightBound1 = r.second;
			if (endLevel > 0) {
				BoundElement leftBound1;
				leftBound1.Exists = true;
				leftBound1.ParagraphIndex = rightBound1.ParagraphIndex;
				leftBound1.ElementIndex = rightBound1.ElementIndex;
				leftBound1.CharIndex = 0;
				cursor.moveToParagraph(r.second.ParagraphIndex);
				cursor.moveTo(r.second.ElementIndex, 0);
				while (endLevel > 0) {
					switch(cursor.element().kind()) {
						case ZLTextElement::START_REVERSED_SEQUENCE_ELEMENT:
							--endLevel;
							leftBound1.ElementIndex = cursor.elementIndex();
							myRanges.push_back(Range(leftBound1, rightBound1));
							rightBound1 = leftBound1;
							break;
						case ZLTextElement::END_REVERSED_SEQUENCE_ELEMENT:
							++endLevel;
							rightBound1.ElementIndex = cursor.elementIndex();
							myRanges.push_back(Range(leftBound1, rightBound1));
							rightBound1 = leftBound1;
							break;
						default:
							break;
					}
					cursor.previousWord();
				}
			}
			myRanges.push_back(Range(leftBound, rightBound1));
		}

		myRangeVectorIsUpToDate = true;
	}
	return myRanges;
}

ZLTextSelectionModel::Range ZLTextSelectionModel::internalRange() const {
	return
		(mySecondBound < myFirstBound) ?
			Range(mySecondBound.After, myFirstBound.Before) :
			Range(myFirstBound.After, mySecondBound.Before);
}

bool ZLTextSelectionModel::Bound::operator < (const Bound &bound) const {
	if (!bound.Before.Exists) {
		return false;
	}
	if (!Before.Exists) {
		return true;
	}

	if (!After.Exists) {
		return false;
	}
	if (!bound.After.Exists) {
		return true;
	}

	if (Before.ParagraphIndex < bound.Before.ParagraphIndex) {
		return true;
	}
	if (Before.ParagraphIndex > bound.Before.ParagraphIndex) {
		return false;
	}

	if (Before.ElementIndex < bound.Before.ElementIndex) {
		return true;
	}
	if (Before.ElementIndex > bound.Before.ElementIndex) {
		return false;
	}

	return Before.CharIndex < bound.Before.CharIndex;
}

bool ZLTextSelectionModel::isEmpty() const {
	if (myIsEmpty) {
		return true;
	}
	Range r = internalRange();
	return !r.first.Exists || !r.second.Exists || (r.first == r.second);
}

void ZLTextSelectionModel::invalidate() {
	myIsValid = false;
}

void ZLTextSelectionModel::update() {
	if (myIsValid) {
		return;
	}
	myIsValid = true;
	setBound(mySecondBound, myStoredX, myStoredY);
	copySelectionToClipboard(ZLDialogManager::CLIPBOARD_SELECTION);
	myTextIsUpToDate = false;
	clearData();
	myRangeVectorIsUpToDate = false;
	myRanges.clear();
}

void ZLTextSelectionModel::createData() const {
	if (!myTextIsUpToDate && !isEmpty()) {
		Range r = internalRange();

		ZLTextWordCursor start = myArea.startCursor();
		start.moveToParagraph(r.first.ParagraphIndex);
		start.moveTo(r.first.ElementIndex, r.first.CharIndex);

		ZLTextWordCursor end = myArea.startCursor();
		end.moveToParagraph(r.second.ParagraphIndex);
		end.moveTo(r.second.ElementIndex, r.second.CharIndex);

		std::set<ZLTextParagraphCursorPtr> pcursors;
		pcursors.insert(start.paragraphCursorPtr());

		ZLTextWordCursor cursor = start;
		while (cursor < end) {
			if (cursor.isEndOfParagraph()) {
				cursor.nextParagraph();
				pcursors.insert(cursor.paragraphCursorPtr());
				myText.append(ZLibrary::EndOfLine);
				continue;
			}
			const ZLTextElement &element = cursor.element();
			switch (element.kind()) {
				case ZLTextElement::WORD_ELEMENT:
				{
					const ZLTextWord &word = (const ZLTextWord&)element;
					if (cursor.sameElementAs(end)) {
						if (start.sameElementAs(end)) {
							int skip = ZLUnicodeUtil::length(word.Data, start.charIndex());
							int length = ZLUnicodeUtil::length(word.Data, end.charIndex()) - skip;
							myText.append(word.Data + skip, length);
						} else {
							myText.append(word.Data, ZLUnicodeUtil::length(word.Data, end.charIndex()));
						}
					} else if (cursor.charIndex() == 0) {
						myText.append(word.Data, word.Size);
					} else /* cursor == start */ {
						int skip = ZLUnicodeUtil::length(word.Data, cursor.charIndex());
						myText.append(word.Data + skip, word.Size - skip);
					}
					break;
				}
				case ZLTextElement::IMAGE_ELEMENT:
					if (myImage.isNull()) {
						myImage = ((const ZLTextImageElement&)element).image();
					}
					break;
				case ZLTextElement::HSPACE_ELEMENT:
				case ZLTextElement::NB_HSPACE_ELEMENT:
					myText += ' ';
					break;
				default:
					break;
			}
			cursor.nextWord();
		}
		if ((cursor == end) && !cursor.isEndOfParagraph() && myImage.isNull()) {
			const ZLTextElement &element = cursor.element();
			if (element.kind() == ZLTextElement::IMAGE_ELEMENT) {
				myImage = ((const ZLTextImageElement&)element).image();
			}
		}

		myCursors.swap(pcursors);
		myTextIsUpToDate = true;
	}
}

const std::string &ZLTextSelectionModel::text() const {
	createData();
	return myText;
}

shared_ptr<ZLImageData> ZLTextSelectionModel::image() const {
	createData();
	return myImage;
}

bool ZLTextSelectionModel::selectWord(int x, int y) {
	clear();

	const ZLTextElementRectangle *rectangle = myArea.elementByCoordinates(x, y);
	if (rectangle == 0) {
		return false;
	}

	int startIndex = 0;
	int endIndex = 1;
	switch (rectangle->Kind) {
		default:
			return false;
		case ZLTextElement::IMAGE_ELEMENT:
			break;
		case ZLTextElement::WORD_ELEMENT:
		{
			ZLTextWordCursor cursor = myArea.startCursor();
			cursor.moveToParagraph(rectangle->ParagraphIndex);
			const ZLTextWord &word = (const ZLTextWord&)cursor.paragraphCursor()[rectangle->ElementIndex];
			ZLUnicodeUtil::Ucs4String ucs4string;
			ZLUnicodeUtil::utf8ToUcs4(ucs4string, word.Data, word.Size);
			startIndex = charIndex(*rectangle, x);
			if (startIndex == word.Length) {
				--startIndex;
			}
			endIndex = startIndex + 1;
			ZLUnicodeUtil::Ucs4Char ch = ucs4string[startIndex];
			if (ZLUnicodeUtil::isLetter(ch) || (('0' <= ch) && (ch <= '9'))) {
				while (--startIndex >= 0) {
					ch = ucs4string[startIndex];
					if (!ZLUnicodeUtil::isLetter(ch) && ((ch < '0') || (ch > '9'))) {
						break;
					}
				}
				++startIndex;
				while (++endIndex <= word.Length) {
					ch = ucs4string[endIndex - 1];
					if (!ZLUnicodeUtil::isLetter(ch) && ((ch < '0') || (ch > '9'))) {
						break;
					}
				}
				--endIndex;
			}
		}
	}

	myFirstBound.Before.Exists = true;
	myFirstBound.Before.ParagraphIndex = rectangle->ParagraphIndex;
	myFirstBound.Before.ElementIndex = rectangle->ElementIndex;
	myFirstBound.Before.CharIndex = startIndex;
	myFirstBound.After = myFirstBound.Before;

	mySecondBound.Before = myFirstBound.Before;
	mySecondBound.Before.CharIndex = endIndex;
	mySecondBound.After = mySecondBound.Before;

	myIsEmpty = false;
	myTextIsUpToDate = false;
	myRangeVectorIsUpToDate = false;

	copySelectionToClipboard(ZLDialogManager::CLIPBOARD_SELECTION);

	return true;
}

void ZLTextSelectionModel::extendWordSelectionToParagraph() {
	clear();

	myFirstBound.Before.ElementIndex = 0;
	myFirstBound.Before.CharIndex = 0;
	myFirstBound.After = myFirstBound.Before;

	ZLTextWordCursor cursor = myArea.startCursor();
	cursor.moveToParagraph(myFirstBound.Before.ParagraphIndex);
	cursor.moveToParagraphEnd();

	mySecondBound.Before.ElementIndex = cursor.elementIndex();
	mySecondBound.Before.CharIndex = 0;
	mySecondBound.After = mySecondBound.Before;

	myIsEmpty = false;
	myTextIsUpToDate = false;
	myRangeVectorIsUpToDate = false;

	copySelectionToClipboard(ZLDialogManager::CLIPBOARD_SELECTION);
}

void ZLTextSelectionModel::copySelectionToClipboard(ZLDialogManager::ClipboardType type) const {
	if (ZLDialogManager::Instance().isClipboardSupported(type)) {
		createData();
		if (!myText.empty()) {
			ZLDialogManager::Instance().setClipboardText(myText, type);
		} else if (!myImage.isNull()) {
			ZLDialogManager::Instance().setClipboardImage(*myImage, type);
		}
	}
}
