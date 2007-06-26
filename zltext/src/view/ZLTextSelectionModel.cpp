/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include <ZLTime.h>
#include <ZLUnicodeUtil.h>
#include <ZLibrary.h>

#include "ZLTextSelectionModel.h"
#include "ZLTextView.h"

SelectionModel::SelectionModel(TextView &view) : myView(view), myIsActive(false), myIsEmpty(true), myDoUpdate(false), myTextIsUpToDate(true) {
}

void SelectionModel::setBound(Bound &bound, int x, int y) {
	if (myView.myTextElementMap.empty()) {
		return;
	}

	TextElementMap::const_iterator it = myView.myTextElementMap.begin();
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
		if (TextElementArea::RangeChecker(x, y)(*it)) {
			bound.Before.ParagraphNumber = bound.After.ParagraphNumber;
			bound.Before.TextElementNumber = bound.After.TextElementNumber;
			bound.Before.Exists = true;
			if (it->Kind == TextElement::WORD_ELEMENT) {
				myView.myStyle.setStyle(it->Style);
				WordCursor cursor = myView.startCursor();
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
			const TextElementArea &previous = *(it - 1);
			bound.Before.ParagraphNumber = previous.ParagraphNumber;
			bound.Before.TextElementNumber = previous.TextElementNumber;
			bound.Before.CharNumber = previous.StartCharNumber + previous.Length;
			bound.Before.Exists = true;
		}
	} else {
		const TextElementArea &back = myView.myTextElementMap.back();
		bound.Before.ParagraphNumber = back.ParagraphNumber;
		bound.Before.TextElementNumber = back.TextElementNumber;
		bound.Before.CharNumber = back.StartCharNumber + back.Length;
		bound.Before.Exists = true;
		bound.After.Exists = false;
	}
}

void SelectionModel::activate(int x, int y) {
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

bool SelectionModel::BoundElement::operator == (const SelectionModel::BoundElement &element) const {
	return
		(Exists == element.Exists) &&
		(ParagraphNumber == element.ParagraphNumber) &&
		(TextElementNumber == element.TextElementNumber) &&
		(CharNumber == element.CharNumber);
}

bool SelectionModel::BoundElement::operator != (const SelectionModel::BoundElement &element) const {
	return !operator == (element);
}

bool SelectionModel::extendTo(int x, int y) {
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

void SelectionModel::deactivate() {
	stopSelectionScrolling();
	myIsActive = false;
	myDoUpdate = false;
}

void SelectionModel::clear() {
	stopSelectionScrolling();
	myIsEmpty = true;
	myIsActive = false;
	myDoUpdate = false;
	myCursors.clear();
	myText.erase();
	myTextIsUpToDate = true;
}

std::pair<SelectionModel::BoundElement,SelectionModel::BoundElement> SelectionModel::range() const {
	return
		(mySecondBound < myFirstBound) ?
		std::pair<BoundElement,BoundElement>(mySecondBound.After, myFirstBound.Before) :
		std::pair<BoundElement,BoundElement>(myFirstBound.After, mySecondBound.Before);
}

bool SelectionModel::Bound::operator < (const Bound &bound) const {
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

bool SelectionModel::isEmpty() const {
	if (myIsEmpty) {
		return true;
	}
	std::pair<BoundElement,BoundElement> r = range();
	return !r.first.Exists || !r.second.Exists || (r.first == r.second);
}

void SelectionModel::startSelectionScrolling(bool forward) {
	if (mySelectionScroller.isNull()) {
		mySelectionScroller = new SelectionScroller(*this);
	}
	SelectionScroller::Direction direction =
		forward ?
			SelectionScroller::SCROLL_FORWARD :
			SelectionScroller::SCROLL_BACKWARD;
	SelectionScroller &scroller = ((SelectionScroller&)*mySelectionScroller);
	if (scroller.direction() != direction) {
		if (scroller.direction() != SelectionScroller::DONT_SCROLL) {
			ZLTimeManager::instance().removeTask(mySelectionScroller);
		}
		((SelectionScroller&)*mySelectionScroller).setDirection(direction);
		ZLTimeManager::instance().addTask(mySelectionScroller, 400);
	}
}

void SelectionModel::stopSelectionScrolling() {
	if (!mySelectionScroller.isNull()) {
		((SelectionScroller&)*mySelectionScroller).setDirection(SelectionScroller::DONT_SCROLL);
		ZLTimeManager::instance().removeTask(mySelectionScroller);
	}
}

void SelectionModel::update() {
	if (myDoUpdate) {
		myDoUpdate = false;
		setBound(mySecondBound, myStoredX, myStoredY);
		myView.copySelectedTextToClipboard(ZLDialogManager::CLIPBOARD_SELECTION);
		myTextIsUpToDate = false;
		myText.erase();
	}
}

void SelectionModel::scrollAndExtend() {
	SelectionScroller::Direction direction =
		((SelectionScroller&)*mySelectionScroller).direction();
	if (direction != SelectionScroller::DONT_SCROLL) {
		myView.scrollPage(direction == SelectionScroller::SCROLL_FORWARD, TextView::SCROLL_LINES, 1);
		myDoUpdate = true;
		myView.repaintView();
	}
}

const std::string &SelectionModel::getText() const {
	if (!myTextIsUpToDate && !isEmpty()) {
		std::pair<BoundElement,BoundElement> r = range();

		WordCursor start = myView.startCursor();
		start.moveToParagraph(r.first.ParagraphNumber);
		start.moveTo(r.first.TextElementNumber, r.first.CharNumber);

		WordCursor end = myView.startCursor();
		end.moveToParagraph(r.second.ParagraphNumber);
		end.moveTo(r.second.TextElementNumber, r.second.CharNumber);

		std::set<ParagraphCursorPtr> pcursors;
		pcursors.insert(start.paragraphCursorPtr());

		for (WordCursor cursor = start; cursor < end; ) {
			const TextElement &element = cursor.element();
			switch (element.kind()) {
				case TextElement::WORD_ELEMENT:
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
				case TextElement::HSPACE_ELEMENT:
					myText += ' ';
					break;
				default:
					break;
			}
			cursor.nextWord();
			if ((cursor < end) && cursor.isEndOfParagraph()) {
				cursor.nextParagraph();
				pcursors.insert(cursor.paragraphCursorPtr());
				myText.append(ZLibrary::EndOfLine);
			}
		}

		myCursors.swap(pcursors);
		myTextIsUpToDate = true;
	}

	return myText;
}

SelectionScroller::SelectionScroller(SelectionModel &selectionModel) : mySelectionModel(selectionModel), myDirection(DONT_SCROLL) {
}

void SelectionScroller::setDirection(Direction direction) {
	myDirection = direction;
}

void SelectionScroller::run() {
	mySelectionModel.scrollAndExtend();
}
