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

#include <ZLUnicodeUtil.h>
#include <ZLApplication.h>

#include <ZLTextModel.h>
#include <ZLTextParagraph.h>

#include "ZLTextView.h"
#include "ZLTextViewStyle.h"
#include "ZLTextLineInfo.h"
#include "ZLTextParagraphCursor.h"
#include "ZLTextWord.h"
#include "ZLTextSelectionModel.h"
#include "ZLTextPositionIndicator.h"

const int ZLTextView::DOUBLE_CLICK_DELAY = 200;

const std::string ZLTextView::TYPE_ID = "ZLTextView";

const std::string &ZLTextView::typeId() const {
	return TYPE_ID;
}

ZLTextView::ZLTextView(ZLPaintContext &context) : ZLView(context), myTextArea(context), myPaintState(NOTHING_TO_PAINT), mySelectionModel(*this), myTreeStateIsFrozen(false), myDoUpdateScrollbar(false) {
}

ZLTextView::~ZLTextView() {
	clear();
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

	myTextArea.setModel(model);

	if (!model.isNull() && (model->paragraphsNumber() != 0)) {
		setStartCursor(ZLTextParagraphCursor::cursor(*model));

		size_t size = model->paragraphsNumber();
		myTextSize.reserve(size + 1);
		myTextSize.push_back(0);
		size_t currentSize = 0;
		for (size_t i= 0; i < size; ++i) {
			const ZLTextParagraph &para = *(*model)[i];
			currentSize += para.characterNumber();
			switch (para.kind()) {
				case ZLTextParagraph::END_OF_TEXT_PARAGRAPH:
					myTextBreaks.push_back(i);
					currentSize = ((currentSize - 1) / 2048 + 1) * 2048;
					break;
				case ZLTextParagraph::END_OF_SECTION_PARAGRAPH:
					currentSize = ((currentSize - 1) / 2048 + 1) * 2048;
					break;
				default:
					break;
			}
			myTextSize.push_back(currentSize);
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
	ZLApplication::Instance().refreshWindow();
}

void ZLTextView::scrollToEndOfText() {
	shared_ptr<ZLTextModel> model = myTextArea.model();
	if (endCursor().isNull() || model.isNull()) {
		return;
	}

	if (endCursor().isEndOfParagraph() &&
			endCursor().paragraphCursor().isLast()) {
		return;
	}

	std::vector<size_t>::const_iterator i = nextBreakIterator();
	if (i == myTextBreaks.end()) {
		gotoParagraph(model->paragraphsNumber(), true);
		myEndCursor.nextParagraph();
	} else {
		gotoParagraph(*i - 1, true);
	}
	myEndCursor.moveToParagraphEnd();
	ZLApplication::Instance().refreshWindow();
}

int ZLTextView::paragraphIndexByCoordinates(int x, int y) const {
	x = visualX(x);
	int paragraphIndex = -1;
	int yBottom = -1;
	int xLeft = context().width() + 1;
	int xRight = -1;

	for (ZLTextElementIterator it = myTextElementMap.begin(); it != myTextElementMap.end(); ++it) {
		if (it->YEnd < y) {
			paragraphIndex = it->ParagraphIndex;
			if (it->YStart > yBottom) {
				yBottom = it->YEnd;
				xLeft = it->XStart;
				xRight = -1;
			}
			xRight = it->XEnd;
			continue;
		}
		if (it->YStart > y) {
			return
				((paragraphIndex == it->ParagraphIndex) &&
				 (xLeft <= x) && (x <= xRight)) ?
				paragraphIndex : -1;
		}
		if (it->XEnd < x) {
			paragraphIndex = it->ParagraphIndex;
			if (it->YStart > yBottom) {
				yBottom = it->YEnd;
				xLeft = it->XStart;
				xRight = -1;
			}
			xRight = it->XEnd;
			continue;
		}
		if (it->XStart > x) {
			return
				((paragraphIndex == it->ParagraphIndex) &&
				 (it->YStart <= yBottom) && (xLeft < x)) ?
				paragraphIndex : -1;
		}
		return it->ParagraphIndex;
	}
	return -1;
}

const ZLTextElementRectangle *ZLTextView::elementByCoordinates(int x, int y) const {
	ZLTextElementIterator it =
		std::find_if(myTextElementMap.begin(), myTextElementMap.end(), ZLTextElementRectangle::RangeChecker(visualX(x), y));
	return (it != myTextElementMap.end()) ? &*it : 0;
}

void ZLTextView::gotoMark(ZLTextMark mark) {
	if (mark.ParagraphIndex < 0) {
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
	if (((int)startCursor().paragraphCursor().index() != mark.ParagraphIndex) ||
			(startCursor().position() > mark)) {
		doRepaint = true;
		gotoParagraph(mark.ParagraphIndex);
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
		ZLApplication::Instance().refreshWindow();
	}
}

void ZLTextView::gotoParagraph(int num, bool end) {
	shared_ptr<ZLTextModel> model = myTextArea.model();
	if (model.isNull()) {
		return;
	}

	if (!startCursor().isNull() &&
			startCursor().isStartOfParagraph() &&
			startCursor().paragraphCursor().isFirst() &&
			(num >= (int)startCursor().paragraphCursor().index()) &&
			!endCursor().isNull() &&
			endCursor().isEndOfParagraph() &&
			endCursor().paragraphCursor().isLast() &&
			(num <= (int)endCursor().paragraphCursor().index())) {
		return;
	}

	if (model->kind() == ZLTextModel::TREE_MODEL) {
		if ((num >= 0) && (num < (int)model->paragraphsNumber())) {
			ZLTextTreeParagraph *tp = (ZLTextTreeParagraph*)(*model)[num];
			if (myTreeStateIsFrozen) {
				int corrected = num;
				ZLTextTreeParagraph *visible = tp;
				for (ZLTextTreeParagraph *parent = tp->parent(); parent != 0; parent = parent->parent()) {
					if (!parent->isOpen()) {
						visible = parent;
					}
				}
				if (visible != tp) {
					for (--corrected; ((corrected > 0) && visible != (*model)[corrected]); --corrected) {
					}
				}
				if (end && (corrected != num)) {
					++corrected;
				}
				num = corrected;
			} else {
				tp->openTree();
				rebuildPaintInfo(true);
			}
		}
	}

	if (end) {
		if ((num > 0) && (num <= (int)model->paragraphsNumber())) {
			moveEndCursor(num);
		}
	} else {
		if ((num >= 0) && (num < (int)model->paragraphsNumber())) {
			moveStartCursor(num);
		}
	}
}

void ZLTextView::gotoPosition(int paragraphIndex, int elementIndex, int charIndex) {
	gotoParagraph(paragraphIndex, false);
	if (!myStartCursor.isNull() && 
			((int)myStartCursor.paragraphCursor().index() == paragraphIndex)) {
		moveStartCursor(paragraphIndex, elementIndex, charIndex);
	}
}

bool ZLTextView::hasMultiSectionModel() const {
	return !myTextBreaks.empty();
}

void ZLTextView::search(const std::string &text, bool ignoreCase, bool wholeText, bool backward, bool thisSectionOnly) {
	shared_ptr<ZLTextModel> model = myTextArea.model();
	if (model.isNull() || text.empty()) {
		return;
	}

	size_t startIndex = 0;
	size_t endIndex = model->paragraphsNumber();
	if (thisSectionOnly) {
		std::vector<size_t>::const_iterator i = nextBreakIterator();
		if (i != myTextBreaks.begin()) {
			startIndex = *(i - 1);
		}
		if (i != myTextBreaks.end()) {
			endIndex = *i;
		}
	}

	model->search(text, startIndex, endIndex, ignoreCase);
	if (!startCursor().isNull()) {
		rebuildPaintInfo(true);
		ZLTextMark position = startCursor().position();
		gotoMark(wholeText ?
							(backward ? model->lastMark() : model->firstMark()) :
							(backward ? model->previousMark(position) : model->nextMark(position)));
		ZLApplication::Instance().refreshWindow();
	}
}

bool ZLTextView::canFindNext() const {
	return !endCursor().isNull() && (myTextArea.model()->nextMark(endCursor().position()).ParagraphIndex > -1);
}

void ZLTextView::findNext() {
	if (!endCursor().isNull()) {
		gotoMark(myTextArea.model()->nextMark(endCursor().position()));
	}
}

bool ZLTextView::canFindPrevious() const {
	return !startCursor().isNull() && (myTextArea.model()->previousMark(startCursor().position()).ParagraphIndex > -1);
}

void ZLTextView::findPrevious() {
	if (!startCursor().isNull()) {
		gotoMark(myTextArea.model()->previousMark(startCursor().position()));
	}
}

bool ZLTextView::onStylusPress(int x, int y) {
	myDoubleClickInfo.update(x, y, true);
	if (myDoubleClickInfo.Count > 10) {
		return true;
	}

	mySelectionModel.deactivate();

	shared_ptr<ZLTextModel> model = myTextArea.model();
  if (model.isNull()) {
	  return false;
	}

	shared_ptr<ZLTextPositionIndicatorInfo> indicatorInfo = this->indicatorInfo();
	if (!indicatorInfo.isNull() &&
			(indicatorInfo->type() == ZLTextPositionIndicatorInfo::FB_INDICATOR) &&
			indicatorInfo->isSensitive()) {
		myTreeStateIsFrozen = true;
		bool indicatorAnswer = positionIndicator()->onStylusPress(x, y);
		myTreeStateIsFrozen = false;
		if (indicatorAnswer) {
			ZLApplication::Instance().refreshWindow();
			return true;
		}
	}

	if (model->kind() == ZLTextModel::TREE_MODEL) {
		ZLTextTreeNodeMap::const_iterator it =
			std::find_if(myTreeNodeMap.begin(), myTreeNodeMap.end(), ZLTextTreeNodeRectangle::RangeChecker(x, y));
		if (it != myTreeNodeMap.end()) {
			int paragraphIndex = it->ParagraphIndex;
			ZLTextTreeParagraph *paragraph = (ZLTextTreeParagraph*)(*model)[paragraphIndex];

			paragraph->open(!paragraph->isOpen());
			rebuildPaintInfo(true);
			preparePaintInfo();
			if (paragraph->isOpen()) {
				int nextParagraphIndex = paragraphIndex + paragraph->fullSize();
				int lastParagraphIndex = endCursor().paragraphCursor().index();
				if (endCursor().isEndOfParagraph()) {
					++lastParagraphIndex;
				}
				if (lastParagraphIndex < nextParagraphIndex) {
					gotoParagraph(nextParagraphIndex, true);
					preparePaintInfo();
				}
			}
			int firstParagraphIndex = startCursor().paragraphCursor().index();
			if (startCursor().isStartOfParagraph()) {
				--firstParagraphIndex;
			}
			if (firstParagraphIndex >= paragraphIndex) {
				gotoParagraph(paragraphIndex);
				preparePaintInfo();
			}
			ZLApplication::Instance().refreshWindow();

			return true;
		}
	}

	return false;
}

void ZLTextView::activateSelection(int x, int y) {
	if (isSelectionEnabled()) {
		mySelectionModel.activate(visualX(x), y);
		ZLApplication::Instance().refreshWindow();
	}
}

bool ZLTextView::onStylusMove(int x, int y) {
	shared_ptr<ZLTextModel> model = myTextArea.model();
	if (!model.isNull()) {
		if (model->kind() == ZLTextModel::TREE_MODEL) {
			ZLTextTreeNodeMap::const_iterator it =
				std::find_if(myTreeNodeMap.begin(), myTreeNodeMap.end(), ZLTextTreeNodeRectangle::RangeChecker(x, y));
			if (it != myTreeNodeMap.end()) {
				ZLApplication::Instance().setHyperlinkCursor(true);
				return true;
			}
		}
		ZLApplication::Instance().setHyperlinkCursor(false);
	}
	return false;
}

bool ZLTextView::onStylusMovePressed(int x, int y) {
	if (mySelectionModel.extendTo(visualX(x), y)) {
		ZLApplication::Instance().refreshWindow();
		copySelectedTextToClipboard(ZLDialogManager::CLIPBOARD_SELECTION);
	}
	return true;
}

void ZLTextView::copySelectedTextToClipboard(ZLDialogManager::ClipboardType type) const {
	if (ZLDialogManager::Instance().isClipboardSupported(type)) {
		std::string text = mySelectionModel.text();
		if (!text.empty()) {
			ZLDialogManager::Instance().setClipboardText(text, type);
		} else {
			shared_ptr<ZLImageData> image = mySelectionModel.image();
			if (!image.isNull()) {
				ZLDialogManager::Instance().setClipboardImage(*image, type);
			}
		}
	}
}

bool ZLTextView::onStylusClick(int x, int y, int count) {
	if (count > 20) {
		return true;
	} else if (count > 10) {
		mySelectionModel.extendWordSelectionToParagraph();
		ZLApplication::Instance().refreshWindow();
		copySelectedTextToClipboard(ZLDialogManager::CLIPBOARD_SELECTION);
		myDoubleClickInfo.Count = 20;
		return true;
	} else if (count > 2) {
		if (mySelectionModel.selectWord(visualX(x), y)) {
			ZLApplication::Instance().refreshWindow();
			copySelectedTextToClipboard(ZLDialogManager::CLIPBOARD_SELECTION);
			myDoubleClickInfo.Count = 10;
			return true;
		} else {
			myDoubleClickInfo.Count = 0;
		}
	} else {
		mySelectionModel.clear();
		ZLApplication::Instance().refreshWindow();
		return false;
	}

	return true;
}

bool ZLTextView::onStylusRelease(int x, int y) {
	myDoubleClickInfo.update(x, y, false);

	if (myDoubleClickInfo.Count > 0) {
		return onStylusClick(x, y, myDoubleClickInfo.Count);
	}

	mySelectionModel.deactivate();
	return false;
}

void ZLTextView::drawString(ZLTextViewStyle &style, int x, int y, const char *str, int len, const ZLTextWord::Mark *mark, int shift, bool rtl) {
	context().setColor(color(style.textStyle()->colorStyle()));
	if (mark == 0) {
		context().drawString(x, y, str, len, rtl);
	} else {
		if (rtl) {
			x += context().stringWidth(str, len, rtl);
		}
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
				if (rtl) {
					x -= context().stringWidth(str + pos, endPos - pos, rtl);
				}
				context().drawString(x, y, str + pos, endPos - pos, rtl);
				if (!rtl) {
					x += context().stringWidth(str + pos, endPos - pos, rtl);
				}
			}
			if (markStart < len) {
				context().setColor(color(ZLTextStyle::HIGHLIGHTED_TEXT));
				{
					int endPos = std::min(markStart + markLen, len);
					if (rtl) {
						x -= context().stringWidth(str + markStart, endPos - markStart, rtl);
					}
					context().drawString(x, y, str + markStart, endPos - markStart, rtl);
					if (!rtl) {
						x += context().stringWidth(str + markStart, endPos - markStart, rtl);
					}
				}
				context().setColor(color(style.textStyle()->colorStyle()));
			}
			pos = markStart + markLen;
		}

		if (pos < len) {
			if (rtl) {
				x -= context().stringWidth(str + pos, len - pos, rtl);
			}
			context().drawString(x, y, str + pos, len - pos, rtl);
		}
	}
}

void ZLTextView::drawWord(ZLTextViewStyle &style, int x, int y, const ZLTextWord &word, int start, int length, bool addHyphenationSign) {
	if ((start == 0) && (length == -1)) {
		drawString(style, x, y, word.Data, word.Size, word.mark(), 0, word.BidiLevel % 2 == 1);
	} else {
		int startPos = ZLUnicodeUtil::length(word.Data, start);
		int endPos = (length == -1) ? word.Size : ZLUnicodeUtil::length(word.Data, start + length);
		if (!addHyphenationSign) {
			drawString(style, x, y, word.Data + startPos, endPos - startPos, word.mark(), startPos, word.BidiLevel % 2 == 1);
		} else {
			std::string substr;
			substr.append(word.Data + startPos, endPos - startPos);
			substr += '-';
			drawString(style, x, y, substr.data(), substr.length(), word.mark(), startPos, word.BidiLevel % 2 == 1);
		}
	}
}

void ZLTextView::clearCaches() {
	rebuildPaintInfo(true);
}

void ZLTextView::highlightParagraph(int paragraphIndex) {
	myTextArea.model()->selectParagraph(paragraphIndex);
	rebuildPaintInfo(true);
}

int ZLTextView::infoSize(const ZLTextLineInfo &info, SizeUnit unit) {
	return (unit == PIXEL_UNIT) ? (info.Height + info.Descent + info.VSpaceAfter) : (info.IsVisible ? 1 : 0);
}

int ZLTextView::textHeight() const {
	shared_ptr<ZLTextPositionIndicatorInfo> indicatorInfo = this->indicatorInfo();
	if (!indicatorInfo.isNull() && (indicatorInfo->type() == ZLTextPositionIndicatorInfo::FB_INDICATOR)) {
		return viewHeight() - indicatorInfo->height() - indicatorInfo->offset();
	} else {
		return viewHeight();
	}
}

void ZLTextView::gotoCharIndex(size_t charIndex) {
	shared_ptr<ZLTextModel> model = myTextArea.model();
	if (model.isNull() || positionIndicator().isNull()) {
		return;
	}

	std::vector<size_t>::const_iterator i = nextBreakIterator();
	const size_t startParagraphIndex = (i != myTextBreaks.begin()) ? *(i - 1) + 1 : 0;
	const size_t endParagraphIndex = (i != myTextBreaks.end()) ? *i : model->paragraphsNumber();
	const size_t fullTextSize = myTextSize[endParagraphIndex] - myTextSize[startParagraphIndex];
	charIndex = std::min(charIndex, fullTextSize - 1);

	std::vector<size_t>::const_iterator j = std::lower_bound(myTextSize.begin(), myTextSize.end(), charIndex + myTextSize[startParagraphIndex]);
	size_t paragraphIndex = j - myTextSize.begin();
	if ((*model)[paragraphIndex]->kind() == ZLTextParagraph::END_OF_SECTION_PARAGRAPH) {
		gotoParagraph(paragraphIndex, true);
		return;
	}

	if (paragraphIndex > startParagraphIndex) {
		--paragraphIndex;
	}
	gotoParagraph(paragraphIndex, false);
	preparePaintInfo();
	if (!positionIndicator().isNull()) {
		size_t endCharIndex = positionIndicator()->sizeOfTextBeforeCursor(endCursor());
		if (endCharIndex > charIndex) {
			while (endCharIndex > charIndex) {
				scrollPage(false, SCROLL_LINES, 1);
				preparePaintInfo();
				if (positionIndicator()->sizeOfTextBeforeCursor(startCursor()) <= myTextSize[startParagraphIndex]) {
					break;
				}
				endCharIndex = positionIndicator()->sizeOfTextBeforeCursor(endCursor());
			}
			if (endCharIndex < charIndex) {
				scrollPage(true, SCROLL_LINES, 1);
			}
		} else {
			int startCharIndex = positionIndicator()->sizeOfTextBeforeCursor(startCursor());
			while (endCharIndex < charIndex) {
				scrollPage(true, SCROLL_LINES, 1);
				preparePaintInfo();
				const int newStartCharIndex = positionIndicator()->sizeOfTextBeforeCursor(startCursor());
				if (newStartCharIndex <= startCharIndex) {
					break;
				}
				startCharIndex = newStartCharIndex;
				endCharIndex = positionIndicator()->sizeOfTextBeforeCursor(endCursor());
			}
			if (endCharIndex > charIndex) {
				scrollPage(false, SCROLL_LINES, 1);
			}
		}
	}
}

void ZLTextView::gotoPage(size_t index) {
	size_t charIndex = (index - 1) * 2048;
	std::vector<size_t>::const_iterator it = std::lower_bound(myTextSize.begin(), myTextSize.end(), charIndex);
	const int paraIndex = it - myTextSize.begin();
	const ZLTextParagraph &para = *(*myTextArea.model())[paraIndex];
	switch (para.kind()) {
		case ZLTextParagraph::END_OF_TEXT_PARAGRAPH:
		case ZLTextParagraph::END_OF_SECTION_PARAGRAPH:
			charIndex = myTextSize[paraIndex - 1];
			break;
		default:
			break;
	}
	gotoCharIndex(charIndex);
}

size_t ZLTextView::pageIndex() {
	if (empty() || positionIndicator().isNull() || endCursor().isNull()) {
		return 0;
	}
	return positionIndicator()->sizeOfTextBeforeCursor(endCursor()) / 2048 + 1;
}

size_t ZLTextView::pageNumber() const {
	if (empty()) {
		return 0;
	}
	std::vector<size_t>::const_iterator i = nextBreakIterator();
	const size_t startIndex = (i != myTextBreaks.begin()) ? *(i - 1) : 0;
	const size_t endIndex = (i != myTextBreaks.end()) ? *i : myTextArea.model()->paragraphsNumber();
	return (myTextSize[endIndex] - myTextSize[startIndex]) / 2048 + 1;
}

void ZLTextView::onScrollbarMoved(Direction direction, size_t full, size_t from, size_t to) {
	if (direction != VERTICAL) {
		return;
	}

	mySelectionModel.deactivate();

  if (myTextArea.model().isNull()) {
	  return;
	}

	if (startCursor().isNull() || endCursor().isNull()) {
		return;
	}

	myTreeStateIsFrozen = true;
	if (from == 0) {
		scrollToStartOfText();
	} else if (to == full) {
		scrollToEndOfText();
	} else {
		gotoCharIndex(to);
	}
	preparePaintInfo();
	myTreeStateIsFrozen = false;
	myDoUpdateScrollbar = false;
	ZLApplication::Instance().refreshWindow();
}

void ZLTextView::onScrollbarStep(Direction direction, int steps) {
	if (direction == VERTICAL) {
		const bool forward = steps > 0;
		scrollPage(forward, SCROLL_LINES, forward ? steps : -steps);
		ZLApplication::Instance().refreshWindow();
	}
}

void ZLTextView::onScrollbarPageStep(Direction direction, int steps) {
	if (direction == VERTICAL) {
		const bool forward = steps > 0;
		scrollPage(forward, NO_OVERLAPPING, forward ? steps : -steps);
		ZLApplication::Instance().refreshWindow();
	}
}

void ZLTextView::forceScrollbarUpdate() {
	myDoUpdateScrollbar = true;
}

ZLTextView::DoubleClickInfo::DoubleClickInfo() {
	Count = 0;
}

void ZLTextView::DoubleClickInfo::update(int x, int y, bool press) {
	ZLTime current;
	int dcDeltaX = X - x;
	int dcDeltaY = Y - y;
	if ((current.millisecondsFrom(Time) < DOUBLE_CLICK_DELAY) &&
			(dcDeltaX > -5) && (dcDeltaX < 5) &&
			(dcDeltaY > -5) && (dcDeltaY < 5)) {
		++Count;
	} else {
		Count = press ? 1 : 0;
	}
	X = x;
	Y = y;
	Time = current;
}
