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

#include <ZLibrary.h>
#include <ZLUnicodeUtil.h>
#include <ZLLanguageUtil.h>
#include <ZLApplication.h>

#include <ZLTextModel.h>
#include <ZLTextParagraph.h>

#include "ZLTextView.h"
#include "ZLTextLineInfo.h"
#include "ZLTextParagraphCursor.h"
#include "ZLTextWord.h"
#include "ZLTextSelectionModel.h"

ZLTextView::ZLTextView(ZLApplication &application, shared_ptr<ZLPaintContext> context) : ZLView(application, context), myPaintState(NOTHING_TO_PAINT), myOldWidth(-1), myOldHeight(-1), myStyle(context), mySelectionModel(*this, application), myTreeStateIsFrozen(false), myDoUpdateScrollbar(false) {
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

void ZLTextView::setModel(shared_ptr<ZLTextModel> model, const std::string &language) {
	clear();

	myModel = model;
	myLanguage = language.empty() ? ZLibrary::Language() : language;
	myStyle.setBaseBidiLevel(ZLLanguageUtil::isRTLLanguage(myLanguage) ? 1 : 0);

	if (!myModel.isNull() && (myModel->paragraphsNumber() != 0)) {
		setStartCursor(ZLTextParagraphCursor::cursor(*myModel, myLanguage));

		size_t size = myModel->paragraphsNumber();
		myTextSize.reserve(size + 1);
		myTextSize.push_back(0);
		size_t currentSize = 0;
		for (size_t i= 0; i < size; ++i) {
			const ZLTextParagraph &para = *(*myModel)[i];
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

int ZLTextView::paragraphIndexByCoordinates(int x, int y) const {
	int storedIndex = -1;
	for (ZLTextElementIterator it = myTextElementMap.begin(); it != myTextElementMap.end(); ++it) {
		if (it->YEnd < y) {
			storedIndex = it->ParagraphIndex;
			continue;
		}
		if (it->YStart > y) {
			return (storedIndex == it->ParagraphIndex) ? storedIndex : -1;
		}
		if (it->XEnd < x) {
			storedIndex = it->ParagraphIndex;
			continue;
		}
		if (it->XStart > x) {
			return (storedIndex == it->ParagraphIndex) ? storedIndex : -1;
		}
		return it->ParagraphIndex;
	}
	return -1;
}

const ZLTextElementArea *ZLTextView::elementByCoordinates(int x, int y) const {
	ZLTextElementIterator it =
		std::find_if(myTextElementMap.begin(), myTextElementMap.end(), ZLTextElementArea::RangeChecker(visualX(x), y));
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
		application().refreshWindow();
	}
}

void ZLTextView::gotoParagraph(int num, bool end) {
	if (myModel.isNull()) {
		return;
	}

	if (myModel->kind() == ZLTextModel::TREE_MODEL) {
		if ((num >= 0) && (num < (int)myModel->paragraphsNumber())) {
			ZLTextTreeParagraph *tp = (ZLTextTreeParagraph*)(*myModel)[num];
			if (myTreeStateIsFrozen) {
				int corrected = num;
				ZLTextTreeParagraph *visible = tp;
				for (ZLTextTreeParagraph *parent = tp->parent(); parent != 0; parent = parent->parent()) {
					if (!parent->isOpen()) {
						visible = parent;
					}
				}
				if (visible != tp) {
					for (--corrected; ((corrected > 0) && visible != (*myModel)[corrected]); --corrected);
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
		if ((num > 0) && (num <= (int)myModel->paragraphsNumber())) {
			moveEndCursor(num);
		}
	} else {
		if ((num >= 0) && (num < (int)myModel->paragraphsNumber())) {
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
	return !endCursor().isNull() && (myModel->nextMark(endCursor().position()).ParagraphIndex > -1);
}

void ZLTextView::findNext() {
	if (!endCursor().isNull()) {
		gotoMark(myModel->nextMark(endCursor().position()));
	}
}

bool ZLTextView::canFindPrevious() const {
	return !startCursor().isNull() && (myModel->previousMark(startCursor().position()).ParagraphIndex > -1);
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
			(indicatorInfo->type() == ZLTextPositionIndicatorInfo::FB_INDICATOR) &&
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
			int paragraphIndex = it->ParagraphIndex;
			ZLTextTreeParagraph *paragraph = (ZLTextTreeParagraph*)(*myModel)[paragraphIndex];

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
			application().refreshWindow();

			return true;
		}
	}

	return false;
}

void ZLTextView::activateSelection(int x, int y) {
	if (isSelectionEnabled()) {
		mySelectionModel.activate(visualX(x), y);
		application().refreshWindow();
	}
}

bool ZLTextView::onStylusMove(int x, int y) {
	if (!myModel.isNull()) {
		if (myModel->kind() == ZLTextModel::TREE_MODEL) {
			ZLTextTreeNodeMap::const_iterator it =
				std::find_if(myTreeNodeMap.begin(), myTreeNodeMap.end(), ZLTextTreeNodeArea::RangeChecker(x, y));
			if (it != myTreeNodeMap.end()) {
				application().setHyperlinkCursor(true);
				return true;
			}
		}
		application().setHyperlinkCursor(false);
	}
	return false;
}

bool ZLTextView::onStylusMovePressed(int x, int y) {
	if (mySelectionModel.extendTo(visualX(x), y)) {
		copySelectedTextToClipboard(ZLDialogManager::CLIPBOARD_SELECTION);
		application().refreshWindow();
	}
	return true;
}

void ZLTextView::copySelectedTextToClipboard(ZLDialogManager::ClipboardType type) const {
	if (ZLDialogManager::instance().isClipboardSupported(type)) {
		std::string text = mySelectionModel.text();
		if (!text.empty()) {
			ZLDialogManager::instance().setClipboardText(text, type);
		}
	}
}

bool ZLTextView::onStylusRelease(int, int) {
	mySelectionModel.deactivate();
	return true;
}

void ZLTextView::drawString(int x, int y, const char *str, int len, const ZLTextWord::Mark *mark, int shift, bool rtl) {
	context().setColor(myStyle.textStyle()->color());
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
				context().setColor(ZLTextStyleCollection::instance().baseStyle().SelectedTextColorOption.value());
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
				context().setColor(myStyle.textStyle()->color());
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

void ZLTextView::drawWord(int x, int y, const ZLTextWord &word, int start, int length, bool addHyphenationSign) {
	if ((start == 0) && (length == -1)) {
		drawString(x, y, word.Data, word.Size, word.mark(), 0, word.BidiLevel % 2 == 1);
	} else {
		int startPos = ZLUnicodeUtil::length(word.Data, start);
		int endPos = (length == -1) ? word.Size : ZLUnicodeUtil::length(word.Data, start + length);
		if (!addHyphenationSign) {
			drawString(x, y, word.Data + startPos, endPos - startPos, word.mark(), startPos, word.BidiLevel % 2 == 1);
		} else {
			std::string substr;
			substr.append(word.Data + startPos, endPos - startPos);
			substr += '-';
			drawString(x, y, substr.data(), substr.length(), word.mark(), startPos, word.BidiLevel % 2 == 1);
		}
	}
}

void ZLTextView::clearCaches() {
	rebuildPaintInfo(true);
}

void ZLTextView::highlightParagraph(int paragraphIndex) {
	myModel->selectParagraph(paragraphIndex);
	rebuildPaintInfo(true);
}

int ZLTextView::infoSize(const ZLTextLineInfo &info, SizeUnit unit) {
	return (unit == PIXEL_UNIT) ? (info.Height + info.Descent + info.VSpaceAfter) : (info.IsVisible ? 1 : 0);
}

int ZLTextView::textAreaHeight() const {
	shared_ptr<ZLTextPositionIndicatorInfo> indicatorInfo = this->indicatorInfo();
	if (!indicatorInfo.isNull() && (indicatorInfo->type() == ZLTextPositionIndicatorInfo::FB_INDICATOR)) {
		return viewHeight() - indicatorInfo->height() - indicatorInfo->offset();
	} else {
		return viewHeight();
	}
}

void ZLTextView::gotoCharIndex(size_t charIndex) {
	if (positionIndicator().isNull()) {
		return;
	}

	std::vector<size_t>::const_iterator i = nextBreakIterator();
	const size_t startParagraphIndex = (i != myTextBreaks.begin()) ? *(i - 1) : 0;
	const size_t endParagraphIndex = (i != myTextBreaks.end()) ? *i : myModel->paragraphsNumber();
	const size_t fullTextSize = myTextSize[endParagraphIndex] - myTextSize[startParagraphIndex];
	charIndex = std::min(charIndex, fullTextSize - 1);

	std::vector<size_t>::const_iterator j = std::lower_bound(myTextSize.begin(), myTextSize.end(), charIndex + myTextSize[startParagraphIndex]);
	size_t paragraphIndex = j - myTextSize.begin();
	if ((*myModel)[paragraphIndex]->kind() == ZLTextParagraph::END_OF_SECTION_PARAGRAPH) {
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
			while (endCharIndex < charIndex) {
				scrollPage(true, SCROLL_LINES, 1);
				preparePaintInfo();
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
	const ZLTextParagraph &para = *(*myModel)[paraIndex];
	switch (para.kind()) {
		case ZLTextParagraph::END_OF_TEXT_PARAGRAPH:
		case ZLTextParagraph::END_OF_SECTION_PARAGRAPH:
			charIndex = myTextSize[paraIndex - 1];
			break;
		default:
			break;
	}
	gotoCharIndex(charIndex);
	application().refreshWindow();
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
	const size_t endIndex = (i != myTextBreaks.end()) ? *i : myModel->paragraphsNumber();
	return (myTextSize[endIndex] - myTextSize[startIndex]) / 2048 + 1;
}

void ZLTextView::onScrollbarMoved(Direction direction, size_t full, size_t from, size_t to) {
	ZLView::Direction dir = ZLView::VERTICAL;
	bool invert = false;
	switch (rotation()) {
		case DEGREES0:
			break;
		case DEGREES90:
			dir = ZLView::HORIZONTAL;
			break;
		case DEGREES180:
			invert = true;
			break;
		case DEGREES270:
			dir = ZLView::HORIZONTAL;
			invert = true;
			break;
	}
	if (direction != dir) {
		return;
	}
	if (invert) {
		size_t tmp = full - from;
		from = full - to;
		to = tmp;
	}

	mySelectionModel.deactivate();

  if (myModel.isNull()) {
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
	application().refreshWindow();
}

void ZLTextView::forceScrollbarUpdate() {
	myDoUpdateScrollbar = true;
}
