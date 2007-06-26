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

#include <algorithm>

#include <ZLUnicodeUtil.h>

#include <ZLTextModel.h>
#include <ZLTextParagraph.h>

#include "ZLTextView.h"
#include "ZLTextLineInfo.h"
#include "ZLTextParagraphCursor.h"
#include "ZLTextWord.h"
#include "ZLTextSelectionModel.h"

TextView::TextView(ZLApplication &application, ZLPaintContext &context) : ZLView(application, context), myPaintState(NOTHING_TO_PAINT), myOldWidth(-1), myOldHeight(-1), myStyle(context), mySelectionModel(*this), myTreeStateIsFrozen(false) {
}

TextView::~TextView() {
	clear();
}

void TextView::clear() {
	mySelectionModel.clear();

	myStartCursor = 0;
	myEndCursor = 0;
	myLineInfos.clear();
	myPaintState = NOTHING_TO_PAINT;

	myTextElementMap.clear();
	myTreeNodeMap.clear();
	myTextSize.clear();
	myTextBreaks.clear();

	ParagraphCursorCache::clear();
}

void TextView::setModel(shared_ptr<ZLTextModel> model, const std::string &name) {
	clear();

	myModel = model;

	if (!myModel.isNull() && (myModel->paragraphsNumber() != 0)) {
		setStartCursor(ParagraphCursor::cursor(*myModel));

		myFileName = name;
		size_t size = myModel->paragraphsNumber();
		myTextSize.reserve(size + 1);
		myTextSize.push_back(0);
		for (size_t i= 0; i < size; ++i) {
			myTextSize.push_back(myTextSize.back() + (*myModel)[i]->textLength());
			if ((*myModel)[i]->kind() == ZLTextParagraph::END_OF_TEXT_PARAGRAPH) {
				myTextBreaks.push_back(i);
			}
		}
	}
}

void TextView::scrollPage(bool forward, ScrollingMode mode, unsigned int value) {
	preparePaintInfo();
	if (myPaintState == READY) {
		myPaintState = forward ? TO_SCROLL_FORWARD : TO_SCROLL_BACKWARD;
		myScrollingMode = mode;
		myOverlappingValue = value;
	}
}

std::vector<size_t>::const_iterator TextView::nextBreakIterator() const {
	return std::lower_bound(myTextBreaks.begin(), myTextBreaks.end(), endCursor().paragraphCursor().index());
}

void TextView::scrollToHome() {
	if (!startCursor().isNull() &&
			startCursor().isStartOfParagraph() &&
			startCursor().paragraphCursor().index() == 0) {
		return;
	}

	gotoParagraph(0, false);
	repaintView();
}

void TextView::scrollToStartOfText() {
	if (endCursor().isNull()) {
		return;
	}

	if (!startCursor().isNull() &&
			startCursor().isStartOfParagraph() &&
			startCursor().paragraphCursor().isFirst()) {
		return;
	}

	std::vector<size_t>::const_iterator i = nextBreakIterator();
	gotoParagraph((i != myTextBreaks.begin()) ? *(i - 1) : 0, false);
	repaintView();
}

void TextView::scrollToEndOfText() {
	if (endCursor().isNull() || myModel.isNull()) {
		return;
	}

	if (endCursor().isEndOfParagraph() &&
			endCursor().paragraphCursor().isLast()) {
		return;
	}

	std::vector<size_t>::const_iterator i = nextBreakIterator();
	if (i == myTextBreaks.end()) {
		gotoParagraph(myModel->paragraphsNumber(), true);
		myEndCursor.nextParagraph();
	} else {
		gotoParagraph(*i - 1, true);
	}
	myEndCursor.moveToParagraphEnd();
	repaintView();
}

int TextView::paragraphIndexByCoordinate(int y) const {
	int indexBefore = -1;
	for (TextElementIterator it = myTextElementMap.begin(); it != myTextElementMap.end(); ++it) {
		if (it->YEnd < y) {
			indexBefore = it->ParagraphNumber;
		} else if ((it->YStart <= y) || (it->ParagraphNumber == indexBefore)) {
			return it->ParagraphNumber;
		} else {
			return -1;
		}
	}
	return -1;
}

const TextElementArea *TextView::elementByCoordinates(int x, int y) const {
	TextElementIterator it =
		std::find_if(myTextElementMap.begin(), myTextElementMap.end(), TextElementArea::RangeChecker(x, y));
	return (it != myTextElementMap.end()) ? &*it : 0;
}

void TextView::gotoMark(ZLTextMark mark) {
	if (mark.ParagraphNumber < 0) {
		return;
	}
	bool doRepaint = false;
	if (startCursor().isNull()) {
		doRepaint = true;
		preparePaintInfo();
	}
	if (startCursor().isNull()) {
		return;
	}
	if (((int)startCursor().paragraphCursor().index() != mark.ParagraphNumber) ||
			(startCursor().position() > mark)) {
		doRepaint = true;
		gotoParagraph(mark.ParagraphNumber);
		preparePaintInfo();
	}
	if (endCursor().isNull()) {
		preparePaintInfo();
	}
	while (mark > endCursor().position()) {
		doRepaint = true;
		scrollPage(true, NO_OVERLAPPING, 0);
		preparePaintInfo();
	}
	if (doRepaint) {
		repaintView();
	}
}

void TextView::gotoParagraph(int num, bool last) {
	if (myModel.isNull()) {
		return;
	}

	if (myModel->kind() == ZLTextModel::TREE_MODEL) {
		if ((num >= 0) && (num < (int)myModel->paragraphsNumber())) {
			ZLTextTreeParagraph *tp = (ZLTextTreeParagraph*)(*myModel)[num];
			if (myTreeStateIsFrozen) {
				int corrected = num;
				ZLTextTreeParagraph *parent = tp->parent();
				while ((corrected > 0) && (parent != 0) && !parent->isOpen()) {
					for (--corrected; ((corrected > 0) && parent != (*myModel)[corrected]); --corrected);
					parent = parent->parent();
				}
				if (last && (corrected != num)) {
					++corrected;
				}
				num = corrected;
			} else {
				tp->openTree();
				rebuildPaintInfo(true);
			}
		}
	}

	if (last) {
		if ((num > 0) && (num <= (int)myModel->paragraphsNumber())) {
			moveEndCursor(num);
		}
	} else {
		if ((num >= 0) && (num < (int)myModel->paragraphsNumber())) {
			moveStartCursor(num);
		}
	}
}

void TextView::gotoPosition(int paragraphNumber, int wordNumber, int charNumber) {
	gotoParagraph(paragraphNumber, false);
	if (!myStartCursor.isNull() && 
			((int)myStartCursor.paragraphCursor().index() == paragraphNumber)) {
		moveStartCursor(paragraphNumber, wordNumber, charNumber);
	}
}

bool TextView::hasMultiSectionModel() const {
	return !myTextBreaks.empty();
}

void TextView::search(const std::string &text, bool ignoreCase, bool wholeText, bool backward, bool thisSectionOnly) {
	if (text.empty()) {
		return;
	}

	size_t startIndex = 0;
	size_t endIndex = myModel->paragraphsNumber();
	if (thisSectionOnly) {
		std::vector<size_t>::const_iterator i = nextBreakIterator();
		if (i != myTextBreaks.begin()) {
			startIndex = *(i - 1);
		}
		if (i != myTextBreaks.end()) {
			endIndex = *i;
		}
	}

	myModel->search(text, startIndex, endIndex, ignoreCase);
	if (!startCursor().isNull()) {
		rebuildPaintInfo(true);
		ZLTextMark position = startCursor().position();
		gotoMark(wholeText ?
							(backward ? myModel->lastMark() : myModel->firstMark()) :
							(backward ? myModel->previousMark(position) : myModel->nextMark(position)));
		repaintView();
	}
}

bool TextView::canFindNext() const {
	return !endCursor().isNull() && (myModel->nextMark(endCursor().position()).ParagraphNumber > -1);
}

void TextView::findNext() {
	if (!endCursor().isNull()) {
		gotoMark(myModel->nextMark(endCursor().position()));
	}
}

bool TextView::canFindPrevious() const {
	return !startCursor().isNull() && (myModel->previousMark(startCursor().position()).ParagraphNumber > -1);
}

void TextView::findPrevious() {
	if (!startCursor().isNull()) {
		gotoMark(myModel->previousMark(startCursor().position()));
	}
}

bool TextView::onStylusPress(int x, int y) {
	mySelectionModel.deactivate();

  if (myModel.isNull()) {
	  return false;
	}

	ZLTextPositionIndicatorStyle &indicatorStyle = ZLTextStyleCollection::instance().indicatorStyle();
	if (indicatorStyle.ShowOption.value() && indicatorStyle.IsSensitiveOption.value()) {
		myTreeStateIsFrozen = true;
		bool indicatorAnswer = positionIndicator().onStylusPress(x, y);
		myTreeStateIsFrozen = false;
		if (indicatorAnswer) {
			return true;
		}
	}

	if (myModel->kind() == ZLTextModel::TREE_MODEL) {
		TreeNodeMap::const_iterator it =
			std::find_if(myTreeNodeMap.begin(), myTreeNodeMap.end(), TreeNodeArea::RangeChecker(x, y));
		if (it != myTreeNodeMap.end()) {
			int paragraphNumber = it->ParagraphNumber;
			ZLTextTreeParagraph *paragraph = (ZLTextTreeParagraph*)(*myModel)[paragraphNumber];

			paragraph->open(!paragraph->isOpen());
			rebuildPaintInfo(true);
			preparePaintInfo();
			if (paragraph->isOpen()) {
				int nextParagraphNumber = paragraphNumber + paragraph->fullSize();
				int lastParagraphNumber = endCursor().paragraphCursor().index();
				if (endCursor().isEndOfParagraph()) {
					++lastParagraphNumber;
				}
				if (lastParagraphNumber < nextParagraphNumber) {
					gotoParagraph(nextParagraphNumber, true);
					preparePaintInfo();
				}
			}
			int firstParagraphNumber = startCursor().paragraphCursor().index();
			if (startCursor().isStartOfParagraph()) {
				--firstParagraphNumber;
			}
			if (firstParagraphNumber >= paragraphNumber) {
				gotoParagraph(paragraphNumber);
				preparePaintInfo();
			}
			repaintView();

			return true;
		}
	}

	return false;
}

void TextView::activateSelection(int x, int y) {
	mySelectionModel.activate(x, y);
	repaintView();
}

bool TextView::onStylusMovePressed(int x, int y) {
	if (mySelectionModel.extendTo(x, y)) {
		copySelectedTextToClipboard(ZLDialogManager::CLIPBOARD_SELECTION);
		repaintView();
	}
	return true;
}

void TextView::copySelectedTextToClipboard(ZLDialogManager::ClipboardType type) const {
	if (ZLDialogManager::instance().isClipboardSupported(type)) {
		std::string text = mySelectionModel.getText();
		if (!text.empty()) {
			ZLDialogManager::instance().setClipboardText(text, type);
		}
	}
}

bool TextView::onStylusRelease(int, int) {
	mySelectionModel.deactivate();
	return true;
}

void TextView::drawString(int x, int y, const char *str, int len, const Word::WordMark *mark, int shift) {
	context().setColor(myStyle.style()->color());
	if (mark == 0) {
		context().drawString(x, y, str, len);
	} else {
		int pos = 0;
		for (; (mark != 0) && (pos < len); mark = mark->next()) {
			int markStart = mark->start() - shift;
			int markLen = mark->length();

			if (markStart < pos) {
				markLen += markStart - pos;
				markStart = pos;
			}

			if (markLen <= 0) {
				continue;
			}

			if (markStart > pos) {
				int endPos = std::min(markStart, len);
				context().drawString(x, y, str + pos, endPos - pos);
				x += context().stringWidth(str + pos, endPos - pos);
			}
			if (markStart < len) {
				context().setColor(ZLTextStyleCollection::instance().baseStyle().SelectedTextColorOption.value());
				{
					int endPos = std::min(markStart + markLen, len);
					context().drawString(x, y, str + markStart, endPos - markStart);
					x += context().stringWidth(str + markStart, endPos - markStart);
				}
				context().setColor(myStyle.style()->color());
			}
			pos = markStart + markLen;
		}

		if (pos < len) {
			context().drawString(x, y, str + pos, len - pos);
		}
	}
}

void TextView::drawWord(int x, int y, const Word &word, int start, int length, bool addHyphenationSign) {
	if ((start == 0) && (length == -1)) {
		drawString(x, y, word.Data, word.Size, word.mark(), 0);
	} else {
		int startPos = ZLUnicodeUtil::length(word.Data, start);
		int endPos = (length == -1) ? word.Size : ZLUnicodeUtil::length(word.Data, start + length);
		if (!addHyphenationSign) {
			drawString(x, y, word.Data + startPos, endPos - startPos, word.mark(), startPos);
		} else {
			std::string substr;
			substr.append(word.Data + startPos, endPos - startPos);
			substr += '-';
			drawString(x, y, substr.data(), substr.length(), word.mark(), startPos);
		}
	}
}

void TextView::clearCaches() {
	rebuildPaintInfo(true);
}

void TextView::highlightParagraph(int paragraphNumber) {
	myModel->selectParagraph(paragraphNumber);
	rebuildPaintInfo(true);
}

int TextView::infoSize(const LineInfo &info, SizeUnit unit) {
	return (unit == PIXEL_UNIT) ? (info.Height + info.Descent + info.VSpaceAfter) : (info.IsVisible ? 1 : 0);
}
