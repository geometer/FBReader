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

#include <ZLUnicodeUtil.h>
#include <ZLApplication.h>

#include <ZLTextModel.h>
#include <ZLTextParagraph.h>

#include "ZLTextView.h"
#include "ZLTextLineInfo.h"
#include "ZLTextParagraphCursor.h"
#include "ZLTextWord.h"
#include "ZLTextSelectionModel.h"

ZLTextView::ZLTextView(ZLApplication &application, shared_ptr<ZLPaintContext> context) : ZLView(application, context), myPaintState(NOTHING_TO_PAINT), myOldWidth(-1), myOldHeight(-1), myStyle(context), mySelectionModel(*this, application), myTreeStateIsFrozen(false) {
}

ZLTextView::~ZLTextView() {
	clear();
}

void ZLTextView::setPaintContext(shared_ptr<ZLPaintContext> context) {
	ZLView::setPaintContext(context);
	myStyle.setPaintContext(context);
}

void ZLTextView::clear() {
	mySelectionModel.clear();

	myStartCursor = 0;
	myEndCursor = 0;
	myLineInfos.clear();
	myPaintState = NOTHING_TO_PAINT;

	myTextElementMap.clear();
	myTreeNodeMap.clear();
	myTextSize.clear();
	myTextBreaks.clear();

	ZLTextParagraphCursorCache::clear();
}

void ZLTextView::setModel(shared_ptr<ZLTextModel> model) {
	clear();

	myModel = model;

	if (!myModel.isNull() && (myModel->paragraphsNumber() != 0)) {
		setStartCursor(ZLTextParagraphCursor::cursor(*myModel));

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

void ZLTextView::scrollPage(bool forward, ScrollingMode mode, unsigned int value) {
	preparePaintInfo();
	if (myPaintState == READY) {
		myPaintState = forward ? TO_SCROLL_FORWARD : TO_SCROLL_BACKWARD;
		myScrollingMode = mode;
		myOverlappingValue = value;
	}
}

std::vector<size_t>::const_iterator ZLTextView::nextBreakIterator() const {
	ZLTextWordCursor cursor = endCursor();
	if (cursor.isNull()) {
		cursor = startCursor();
	}
	if (cursor.isNull()) {
		return myTextBreaks.begin();
	}
	return std::lower_bound(myTextBreaks.begin(), myTextBreaks.end(), cursor.paragraphCursor().index());
}

void ZLTextView::scrollToStartOfText() {
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
	application().refreshWindow();
}

void ZLTextView::scrollToEndOfText() {
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
	application().refreshWindow();
}

int ZLTextView::paragraphIndexByCoordinate(int y) const {
	int indexBefore = -1;
	for (ZLTextElementIterator it = myTextElementMap.begin(); it != myTextElementMap.end(); ++it) {
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

const ZLTextElementArea *ZLTextView::elementByCoordinates(int x, int y) const {
	ZLTextElementIterator it =
		std::find_if(myTextElementMap.begin(), myTextElementMap.end(), ZLTextElementArea::RangeChecker(x, y));
	return (it != myTextElementMap.end()) ? &*it : 0;
}

void ZLTextView::gotoMark(ZLTextMark mark) {
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
		application().refreshWindow();
	}
}

void ZLTextView::gotoParagraph(int num, bool last) {
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

void ZLTextView::gotoPosition(int paragraphNumber, int wordNumber, int charNumber) {
	gotoParagraph(paragraphNumber, false);
	if (!myStartCursor.isNull() && 
			((int)myStartCursor.paragraphCursor().index() == paragraphNumber)) {
		moveStartCursor(paragraphNumber, wordNumber, charNumber);
	}
}

bool ZLTextView::hasMultiSectionModel() const {
	return !myTextBreaks.empty();
}

void ZLTextView::search(const std::string &text, bool ignoreCase, bool wholeText, bool backward, bool thisSectionOnly) {
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
		application().refreshWindow();
	}
}

bool ZLTextView::canFindNext() const {
	return !endCursor().isNull() && (myModel->nextMark(endCursor().position()).ParagraphNumber > -1);
}

void ZLTextView::findNext() {
	if (!endCursor().isNull()) {
		gotoMark(myModel->nextMark(endCursor().position()));
	}
}

bool ZLTextView::canFindPrevious() const {
	return !startCursor().isNull() && (myModel->previousMark(startCursor().position()).ParagraphNumber > -1);
}

void ZLTextView::findPrevious() {
	if (!startCursor().isNull()) {
		gotoMark(myModel->previousMark(startCursor().position()));
	}
}

bool ZLTextView::onStylusPress(int x, int y) {
	mySelectionModel.deactivate();

  if (myModel.isNull()) {
	  return false;
	}

	shared_ptr<ZLTextPositionIndicatorInfo> indicatorInfo = this->indicatorInfo();
	if (!indicatorInfo.isNull() &&
			indicatorInfo->isVisible() &&
			indicatorInfo->isSensitive()) {
		myTreeStateIsFrozen = true;
		bool indicatorAnswer = positionIndicator()->onStylusPress(x, y);
		myTreeStateIsFrozen = false;
		if (indicatorAnswer) {
			application().refreshWindow();
			return true;
		}
	}

	if (myModel->kind() == ZLTextModel::TREE_MODEL) {
		ZLTextTreeNodeMap::const_iterator it =
			std::find_if(myTreeNodeMap.begin(), myTreeNodeMap.end(), ZLTextTreeNodeArea::RangeChecker(x, y));
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
			application().refreshWindow();

			return true;
		}
	}

	return false;
}

void ZLTextView::activateSelection(int x, int y) {
	if (isSelectionEnabled()) {
		mySelectionModel.activate(x, y);
		application().refreshWindow();
	}
}

bool ZLTextView::onStylusMovePressed(int x, int y) {
	if (mySelectionModel.extendTo(x, y)) {
		copySelectedTextToClipboard(ZLDialogManager::CLIPBOARD_SELECTION);
		application().refreshWindow();
	}
	return true;
}

void ZLTextView::copySelectedTextToClipboard(ZLDialogManager::ClipboardType type) const {
	if (ZLDialogManager::instance().isClipboardSupported(type)) {
		std::string text = mySelectionModel.getText();
		if (!text.empty()) {
			ZLDialogManager::instance().setClipboardText(text, type);
		}
	}
}

bool ZLTextView::onStylusRelease(int, int) {
	mySelectionModel.deactivate();
	return true;
}

void ZLTextView::drawString(int x, int y, const char *str, int len, const ZLTextWord::Mark *mark, int shift) {
	context().setColor(myStyle.textStyle()->color());
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
				context().setColor(myStyle.textStyle()->color());
			}
			pos = markStart + markLen;
		}

		if (pos < len) {
			context().drawString(x, y, str + pos, len - pos);
		}
	}
}

void ZLTextView::drawWord(int x, int y, const ZLTextWord &word, int start, int length, bool addHyphenationSign) {
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

void ZLTextView::clearCaches() {
	rebuildPaintInfo(true);
}

void ZLTextView::highlightParagraph(int paragraphNumber) {
	myModel->selectParagraph(paragraphNumber);
	rebuildPaintInfo(true);
}

int ZLTextView::infoSize(const ZLTextLineInfo &info, SizeUnit unit) {
	return (unit == PIXEL_UNIT) ? (info.Height + info.Descent + info.VSpaceAfter) : (info.IsVisible ? 1 : 0);
}

int ZLTextView::textAreaHeight() const {
	shared_ptr<ZLTextPositionIndicatorInfo> indicatorInfo = this->indicatorInfo();
	if (!indicatorInfo.isNull() && indicatorInfo->isVisible()) {
		return viewHeight() - indicatorInfo->height() - indicatorInfo->offset();
	} else {
		return viewHeight();
	}
}
/*
 *
size_t ZLTextView::PositionIndicator::sizeOfTextBeforeParagraph(size_t paragraphNumber) const {
	return myTextView.myTextSize[paragraphNumber] - myTextView.myTextSize[startTextIndex()];
}

size_t ZLTextView::PositionIndicator::sizeOfParagraph(size_t paragraphNumber) const {
	return myTextView.myTextSize[paragraphNumber + 1] - myTextView.myTextSize[paragraphNumber];
}
*/


void ZLTextView::gotoPage(size_t index) {
	const size_t symbolIndex = index * 2048 - 128;
	std::vector<size_t>::const_iterator it = std::lower_bound(myTextSize.begin(), myTextSize.end(), symbolIndex);
	std::vector<size_t>::const_iterator i = nextBreakIterator();
	const size_t startIndex = (i != myTextBreaks.begin()) ? *(i - 1) : 0;
	const size_t endIndex = (i != myTextBreaks.end()) ? *i : myModel->paragraphsNumber();
	size_t paragraphNumber = std::min((size_t)(it - myTextSize.begin()), endIndex) - 1;
	gotoParagraph(paragraphNumber, true);
	preparePaintInfo();
	const ZLTextWordCursor &cursor = endCursor();
	if (!cursor.isNull() && (paragraphNumber == cursor.paragraphCursor().index())) {
		if (!cursor.paragraphCursor().isLast() || !cursor.isEndOfParagraph()) {
			size_t paragraphLength = cursor.paragraphCursor().paragraphLength();
			if (paragraphLength > 0) {
				size_t wordNum =
					(myTextSize[startIndex] + symbolIndex - myTextSize[paragraphNumber]) *
					paragraphLength / (myTextSize[endIndex] - myTextSize[startIndex]);
				moveEndCursor(cursor.paragraphCursor().index(), wordNum, 0);
			}
		}
	}
}

size_t ZLTextView::pageNumber() const {
	if (empty()) {
		return 0;
	}
	std::vector<size_t>::const_iterator i = nextBreakIterator();
	const size_t startIndex = (i != myTextBreaks.begin()) ? *(i - 1) : 0;
	const size_t endIndex = (i != myTextBreaks.end()) ? *i : myModel->paragraphsNumber();
	return (myTextSize[endIndex] - myTextSize[startIndex]) / 2048 + 1;
}
