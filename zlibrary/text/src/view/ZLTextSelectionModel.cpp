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

#include <ZLApplication.h>
#include <ZLTime.h>
#include <ZLUnicodeUtil.h>
#include <ZLibrary.h>

#include "ZLTextSelectionModel.h"
#include "ZLTextView.h"

ZLTextSelectionModel::ZLTextSelectionModel(ZLTextView &view, ZLApplication &application) : myView(view), myApplication(application), myIsActive(false), myIsEmpty(true), myDoUpdate(false), myTextIsUpToDate(true) {
}

void ZLTextSelectionModel::setBound(Bound &bound, int x, int y) {
	if (myView.myTextElementMap.empty()) {
		return;
	}

	ZLTextElementMap::const_iterator it = myView.myTextElementMap.begin();
	for (; it != myView.myTextElementMap.end(); ++it) {
		if ((it->YStart > y) || ((it->YEnd > y) && (it->XEnd > x))) {
			break;
		}
	}

	if (it != myView.myTextElementMap.end()) {
		bound.After.ParagraphNumber = it->ParagraphNumber;
		bound.After.TextElementNumber = it->TextElementNumber;
		bound.After.Exists = true;
		bound.After.CharNumber = it->StartCharNumber;
		if (ZLTextElementArea::RangeChecker(x, y)(*it)) {
			bound.Before.ParagraphNumber = bound.After.ParagraphNumber;
			bound.Before.TextElementNumber = bound.After.TextElementNumber;
			bound.Before.Exists = true;
			if (it->Kind == ZLTextElement::WORD_ELEMENT) {
				myView.myStyle.setTextStyle(it->Style);
				ZLTextWordCursor cursor = myView.startCursor();
				cursor.moveToParagraph(it->ParagraphNumber);
				const ZLTextWord &word = (const ZLTextWord&)cursor.paragraphCursor()[it->TextElementNumber];
				const int deltaX = x - it->XStart;
				const int len = it->Length;
				const int start = it->StartCharNumber;
				int diff = deltaX;
				int previousDiff = diff;
				int index;
				for (index = 0; (index < len) && (diff > 0); ++index) {
					previousDiff = diff;
					diff = deltaX - myView.myStyle.wordWidth(word, start, index + 1);
				}
				if (previousDiff + diff < 0) {
					--index;
				}
				bound.After.CharNumber = start + index;
				bound.Before.CharNumber = bound.After.CharNumber;
			}
		} else if (it == myView.myTextElementMap.begin()) {
			bound.Before.Exists = false;
		} else {
			const ZLTextElementArea &previous = *(it - 1);
			bound.Before.ParagraphNumber = previous.ParagraphNumber;
			bound.Before.TextElementNumber = previous.TextElementNumber;
			bound.Before.CharNumber = previous.StartCharNumber + previous.Length;
			bound.Before.Exists = true;
		}
	} else {
		const ZLTextElementArea &back = myView.myTextElementMap.back();
		bound.Before.ParagraphNumber = back.ParagraphNumber;
		bound.Before.TextElementNumber = back.TextElementNumber;
		bound.Before.CharNumber = back.StartCharNumber + back.Length;
		bound.Before.Exists = true;
		bound.After.Exists = false;
	}
}

void ZLTextSelectionModel::activate(int x, int y) {
	if (myView.myTextElementMap.empty()) {
		return;
	}

	myIsActive = true;
	myIsEmpty = false;
	setBound(myFirstBound, x, y);
	mySecondBound = myFirstBound;
	myCursors.clear();
	myText.erase();
	myTextIsUpToDate = true;
}

bool ZLTextSelectionModel::BoundElement::operator == (const ZLTextSelectionModel::BoundElement &element) const {
	return
		(Exists == element.Exists) &&
		(ParagraphNumber == element.ParagraphNumber) &&
		(TextElementNumber == element.TextElementNumber) &&
		(CharNumber == element.CharNumber);
}

bool ZLTextSelectionModel::BoundElement::operator != (const ZLTextSelectionModel::BoundElement &element) const {
	return !operator == (element);
}

bool ZLTextSelectionModel::extendTo(int x, int y) {
	if (!myIsActive || myView.myTextElementMap.empty()) {
		return false;
	}

	std::pair<BoundElement,BoundElement> oldRange = range();
	setBound(mySecondBound, x, y);
	std::pair<BoundElement,BoundElement> newRange = range();
	myStoredX = x;
	myStoredY = y;

	if (!mySecondBound.Before.Exists) {
		startSelectionScrolling(false);
	} else if (!mySecondBound.After.Exists) {
		startSelectionScrolling(true);
	} else {
		stopSelectionScrolling();
	}

	if ((oldRange.first != newRange.first) || (oldRange.second != newRange.second)) {
		myTextIsUpToDate = false;
		myText.erase();
		return true;
	}
	return false;
}

void ZLTextSelectionModel::deactivate() {
	stopSelectionScrolling();
	myIsActive = false;
	myDoUpdate = false;
}

void ZLTextSelectionModel::clear() {
	stopSelectionScrolling();
	myIsEmpty = true;
	myIsActive = false;
	myDoUpdate = false;
	myCursors.clear();
	myText.erase();
	myTextIsUpToDate = true;
}

std::pair<ZLTextSelectionModel::BoundElement,ZLTextSelectionModel::BoundElement> ZLTextSelectionModel::range() const {
	return
		(mySecondBound < myFirstBound) ?
		std::pair<BoundElement,BoundElement>(mySecondBound.After, myFirstBound.Before) :
		std::pair<BoundElement,BoundElement>(myFirstBound.After, mySecondBound.Before);
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

	if (Before.ParagraphNumber < bound.Before.ParagraphNumber) {
		return true;
	}
	if (Before.ParagraphNumber > bound.Before.ParagraphNumber) {
		return false;
	}

	if (Before.TextElementNumber < bound.Before.TextElementNumber) {
		return true;
	}
	if (Before.TextElementNumber > bound.Before.TextElementNumber) {
		return false;
	}

	return Before.CharNumber < bound.Before.CharNumber;
}

bool ZLTextSelectionModel::isEmpty() const {
	if (myIsEmpty) {
		return true;
	}
	std::pair<BoundElement,BoundElement> r = range();
	return !r.first.Exists || !r.second.Exists || (r.first == r.second);
}

void ZLTextSelectionModel::startSelectionScrolling(bool forward) {
	if (mySelectionScroller.isNull()) {
		mySelectionScroller = new ZLTextSelectionScroller(*this);
	}
	ZLTextSelectionScroller::Direction direction =
		forward ?
			ZLTextSelectionScroller::SCROLL_FORWARD :
			ZLTextSelectionScroller::SCROLL_BACKWARD;
	ZLTextSelectionScroller &scroller = ((ZLTextSelectionScroller&)*mySelectionScroller);
	if (scroller.direction() != direction) {
		if (scroller.direction() != ZLTextSelectionScroller::DONT_SCROLL) {
			ZLTimeManager::instance().removeTask(mySelectionScroller);
		}
		((ZLTextSelectionScroller&)*mySelectionScroller).setDirection(direction);
		ZLTimeManager::instance().addTask(mySelectionScroller, 400);
	}
}

void ZLTextSelectionModel::stopSelectionScrolling() {
	if (!mySelectionScroller.isNull()) {
		((ZLTextSelectionScroller&)*mySelectionScroller).setDirection(ZLTextSelectionScroller::DONT_SCROLL);
		ZLTimeManager::instance().removeTask(mySelectionScroller);
	}
}

void ZLTextSelectionModel::update() {
	if (!myView.isSelectionEnabled()) {
		clear();
	} else if (myDoUpdate) {
		myDoUpdate = false;
		setBound(mySecondBound, myStoredX, myStoredY);
		myView.copySelectedTextToClipboard(ZLDialogManager::CLIPBOARD_SELECTION);
		myTextIsUpToDate = false;
		myText.erase();
	}
}

void ZLTextSelectionModel::scrollAndExtend() {
	ZLTextSelectionScroller::Direction direction =
		((ZLTextSelectionScroller&)*mySelectionScroller).direction();
	if (direction != ZLTextSelectionScroller::DONT_SCROLL) {
		myView.scrollPage(direction == ZLTextSelectionScroller::SCROLL_FORWARD, ZLTextView::SCROLL_LINES, 1);
		myDoUpdate = true;
		myApplication.refreshWindow();
	}
}

const std::string &ZLTextSelectionModel::getText() const {
	if (!myTextIsUpToDate && !isEmpty()) {
		std::pair<BoundElement,BoundElement> r = range();

		ZLTextWordCursor start = myView.startCursor();
		start.moveToParagraph(r.first.ParagraphNumber);
		start.moveTo(r.first.TextElementNumber, r.first.CharNumber);

		ZLTextWordCursor end = myView.startCursor();
		end.moveToParagraph(r.second.ParagraphNumber);
		end.moveTo(r.second.TextElementNumber, r.second.CharNumber);

		std::set<ZLTextParagraphCursorPtr> pcursors;
		pcursors.insert(start.paragraphCursorPtr());

		for (ZLTextWordCursor cursor = start; cursor < end; ) {
			if ((cursor < end) && cursor.isEndOfParagraph()) {
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
							int skip = ZLUnicodeUtil::length(word.Data, start.charNumber());
							int length = ZLUnicodeUtil::length(word.Data, end.charNumber()) - skip;
							myText.append(word.Data + skip, length);
						} else {
							myText.append(word.Data, ZLUnicodeUtil::length(word.Data, end.charNumber()));
						}
					} else if (cursor.charNumber() == 0) {
						myText.append(word.Data, word.Size);
					} else /* cursor == start */ {
						int skip = ZLUnicodeUtil::length(word.Data, cursor.charNumber());
						myText.append(word.Data + skip, word.Size - skip);
					}
					break;
				}
				case ZLTextElement::HSPACE_ELEMENT:
					myText += ' ';
					break;
				case ZLTextElement::NB_HSPACE_ELEMENT:
					myText += "\xC2\x20"; // &nbsp;
					break;
				default:
					break;
			}
			cursor.nextWord();
		}

		myCursors.swap(pcursors);
		myTextIsUpToDate = true;
	}

	return myText;
}

ZLTextSelectionScroller::ZLTextSelectionScroller(ZLTextSelectionModel &selectionModel) : mySelectionModel(selectionModel), myDirection(DONT_SCROLL) {
}

void ZLTextSelectionScroller::setDirection(Direction direction) {
	myDirection = direction;
}

void ZLTextSelectionScroller::run() {
	mySelectionModel.scrollAndExtend();
}
