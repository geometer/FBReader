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

#include <algorithm>

#include <ZLApplication.h>
#include <ZLTimeManager.h>

#include <ZLTextModel.h>
#include <ZLTextParagraph.h>

#include "ZLTextView.h"
#include "ZLTextSelectionScroller.h"
#include "ZLTextPositionIndicator.h"
#include "../area/ZLTextSelectionModel.h"
#include "../area/ZLTextLineInfo.h"
#include "../area/ZLTextParagraphCursor.h"
#include "../area/ZLTextWord.h"
#include "../area/ZLTextAreaStyle.h"

const ZLTypeId ZLTextView::TYPE_ID(ZLView::TYPE_ID);

const ZLTypeId &ZLTextView::typeId() const {
	return TYPE_ID;
}

ZLTextView::ZLTextView(ZLPaintContext &context) :
	ZLView(context),
	myTextAreaController(context, *this),
	myTreeStateIsFrozen(false),
	myDoUpdateScrollbar(false),
	myDoubleClickInfo(*this) {
}

ZLTextView::~ZLTextView() {
	clear();
}

void ZLTextView::clear() {
	myTextAreaController.clear();

	myTextSize.clear();
	myTextBreaks.clear();
}

void ZLTextView::setModel(shared_ptr<ZLTextModel> model) {
	clear();

	myTextAreaController.setModel(model);

	if (!model.isNull() && (model->paragraphsNumber() != 0)) {
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

std::vector<size_t>::const_iterator ZLTextView::nextBreakIterator() const {
	ZLTextWordCursor cursor = textArea().endCursor();
	if (cursor.isNull()) {
		cursor = textArea().startCursor();
	}
	if (cursor.isNull()) {
		return myTextBreaks.begin();
	}
	return std::lower_bound(myTextBreaks.begin(), myTextBreaks.end(), cursor.paragraphCursor().index());
}

void ZLTextView::scrollToStartOfText() {
	if (textArea().endCursor().isNull()) {
		return;
	}

	const ZLTextWordCursor &startCursor = textArea().startCursor();
	if (!startCursor.isNull() &&
			startCursor.isStartOfParagraph() &&
			startCursor.paragraphCursor().isFirst()) {
		return;
	}

	std::vector<size_t>::const_iterator i = nextBreakIterator();
	gotoParagraph((i != myTextBreaks.begin()) ? *(i - 1) : 0, false);
	ZLApplication::Instance().refreshWindow();
}

void ZLTextView::scrollToEndOfText() {
	shared_ptr<ZLTextModel> model = textArea().model();
	if (textArea().endCursor().isNull() || model.isNull()) {
		return;
	}

	if (textArea().endCursor().isEndOfParagraph() &&
			textArea().endCursor().paragraphCursor().isLast()) {
		return;
	}

	std::vector<size_t>::const_iterator i = nextBreakIterator();
	if (i == myTextBreaks.end()) {
		gotoParagraph(model->paragraphsNumber(), true);
		myTextAreaController.area().myEndCursor.nextParagraph();
	} else {
		gotoParagraph(*i - 1, true);
	}
	myTextAreaController.area().myEndCursor.moveToParagraphEnd();
	ZLApplication::Instance().refreshWindow();
}

void ZLTextView::gotoMark(ZLTextMark mark) {
	if (mark.ParagraphIndex < 0) {
		return;
	}
	bool doRepaint = false;
	if (textArea().startCursor().isNull()) {
		doRepaint = true;
		preparePaintInfo();
	}
	if (textArea().startCursor().isNull()) {
		return;
	}
	if (((int)textArea().startCursor().paragraphCursor().index() != mark.ParagraphIndex) ||
			(textArea().startCursor().position() > mark)) {
		doRepaint = true;
		gotoParagraph(mark.ParagraphIndex);
		preparePaintInfo();
	}
	if (textArea().endCursor().isNull()) {
		preparePaintInfo();
	}
	while (mark > textArea().endCursor().position()) {
		doRepaint = true;
		scrollPage(true, ZLTextAreaController::NO_OVERLAPPING, 0);
		preparePaintInfo();
	}
	if (doRepaint) {
		ZLApplication::Instance().refreshWindow();
	}
}

void ZLTextView::gotoParagraph(int num, bool end) {
	shared_ptr<ZLTextModel> model = textArea().model();
	if (model.isNull()) {
		return;
	}

	const ZLTextWordCursor &startCursor = textArea().startCursor();
	const ZLTextWordCursor &endCursor = textArea().endCursor();
	if (!startCursor.isNull() &&
			startCursor.isStartOfParagraph() &&
			startCursor.paragraphCursor().isFirst() &&
			(num >= (int)startCursor.paragraphCursor().index()) &&
			!endCursor.isNull() &&
			endCursor.isEndOfParagraph() &&
			endCursor.paragraphCursor().isLast() &&
			(num <= (int)endCursor.paragraphCursor().index())) {
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
				myTextAreaController.rebuildPaintInfo(true);
			}
		}
	}

	if (end) {
		if ((num > 0) && (num <= (int)model->paragraphsNumber())) {
			myTextAreaController.moveEndCursor(num);
		}
	} else {
		if ((num >= 0) && (num < (int)model->paragraphsNumber())) {
			myTextAreaController.moveStartCursor(num);
		}
	}
}

void ZLTextView::gotoPosition(int paragraphIndex, int elementIndex, int charIndex) {
	gotoParagraph(paragraphIndex, false);
	const ZLTextWordCursor &startCursor = textArea().startCursor();
	if (!startCursor.isNull() && 
			((int)startCursor.paragraphCursor().index() == paragraphIndex)) {
		myTextAreaController.moveStartCursor(paragraphIndex, elementIndex, charIndex);
	}
}

bool ZLTextView::hasMultiSectionModel() const {
	return !myTextBreaks.empty();
}

void ZLTextView::search(const std::string &text, bool ignoreCase, bool wholeText, bool backward, bool thisSectionOnly) {
	shared_ptr<ZLTextModel> model = textArea().model();
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
	const ZLTextWordCursor &startCursor = textArea().startCursor();
	if (!startCursor.isNull()) {
		myTextAreaController.rebuildPaintInfo(true);
		ZLTextMark position = startCursor.position();
		gotoMark(wholeText ?
							(backward ? model->lastMark() : model->firstMark()) :
							(backward ? model->previousMark(position) : model->nextMark(position)));
		ZLApplication::Instance().refreshWindow();
	}
}

bool ZLTextView::canFindNext() const {
	return !textArea().endCursor().isNull() && (textArea().model()->nextMark(textArea().endCursor().position()).ParagraphIndex > -1);
}

void ZLTextView::findNext() {
	if (!textArea().endCursor().isNull()) {
		gotoMark(textArea().model()->nextMark(textArea().endCursor().position()));
	}
}

bool ZLTextView::canFindPrevious() const {
	const ZLTextWordCursor &startCursor = textArea().startCursor();
	return
		!startCursor.isNull() &&
		(textArea().model()->previousMark(startCursor.position()).ParagraphIndex > -1);
}

void ZLTextView::findPrevious() {
	const ZLTextWordCursor &startCursor = textArea().startCursor();
	if (!startCursor.isNull()) {
		gotoMark(textArea().model()->previousMark(startCursor.position()));
	}
}

bool ZLTextView::onStylusPress(int x, int y) {
	stopSelectionScrolling();

	myDoubleClickInfo.update(x, y, true);
	if (myDoubleClickInfo.Count > 10) {
		return true;
	}

	myTextAreaController.area().selectionModel().deactivate();

	shared_ptr<ZLTextModel> model = textArea().model();
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
		const ZLTextTreeNodeRectangle *node = textArea().treeNodeByCoordinates(x, y);
		if (node != 0) {
			int paragraphIndex = node->ParagraphIndex;
			ZLTextTreeParagraph *paragraph = (ZLTextTreeParagraph*)(*model)[paragraphIndex];

			paragraph->open(!paragraph->isOpen());
			myTextAreaController.rebuildPaintInfo(true);
			preparePaintInfo();
			if (paragraph->isOpen()) {
				int nextParagraphIndex = paragraphIndex + paragraph->fullSize();
				int lastParagraphIndex = textArea().endCursor().paragraphCursor().index();
				if (textArea().endCursor().isEndOfParagraph()) {
					++lastParagraphIndex;
				}
				if (lastParagraphIndex < nextParagraphIndex) {
					gotoParagraph(nextParagraphIndex, true);
					preparePaintInfo();
				}
			}
			const ZLTextWordCursor &startCursor = textArea().startCursor();
			int firstParagraphIndex = startCursor.paragraphCursor().index();
			if (startCursor.isStartOfParagraph()) {
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
		myTextAreaController.area().selectionModel().activate(textArea().realX(x), y);
		ZLApplication::Instance().refreshWindow();
	}
}

bool ZLTextView::onStylusMove(int x, int y) {
	shared_ptr<ZLTextModel> model = textArea().model();
	if (!model.isNull()) {
		if (model->kind() == ZLTextModel::TREE_MODEL) {
			const ZLTextTreeNodeRectangle *node =
				textArea().treeNodeByCoordinates(x, y);
			if (node != 0) {
				ZLApplication::Instance().setHyperlinkCursor(true);
				return true;
			}
		}
		ZLApplication::Instance().setHyperlinkCursor(false);
	}
	return false;
}

bool ZLTextView::onStylusMovePressed(int x, int y) {
	switch (myTextAreaController.area().selectionModel().extendTo(textArea().realX(x), y)) {
		case ZLTextSelectionModel::BOUND_NOT_CHANGED:
			stopSelectionScrolling();
			break;
		case ZLTextSelectionModel::BOUND_CHANGED:
			stopSelectionScrolling();
			ZLApplication::Instance().refreshWindow();
			break;
		case ZLTextSelectionModel::BOUND_OVER_BEFORE:
			startSelectionScrolling(false);
			ZLApplication::Instance().refreshWindow();
			break;
		case ZLTextSelectionModel::BOUND_OVER_AFTER:
			startSelectionScrolling(true);
			ZLApplication::Instance().refreshWindow();
			break;
	}
	return true;
}

bool ZLTextView::onStylusClick(int x, int y, int count) {
	if (count > 20) {
		return true;
	} else if (count > 10) {
		myTextAreaController.area().selectionModel().extendWordSelectionToParagraph();
		ZLApplication::Instance().refreshWindow();
		myDoubleClickInfo.Count = 20;
		return true;
	} else if (count > 2) {
		if (myTextAreaController.area().selectionModel().selectWord(textArea().realX(x), y)) {
			ZLApplication::Instance().refreshWindow();
			myDoubleClickInfo.Count = 10;
			return true;
		} else {
			myDoubleClickInfo.Count = 0;
		}
	} else {
		myTextAreaController.area().selectionModel().clear();
		ZLApplication::Instance().refreshWindow();
		return false;
	}

	return true;
}

bool ZLTextView::onStylusRelease(int x, int y) {
	stopSelectionScrolling();

	myDoubleClickInfo.update(x, y, false);

	shared_ptr<ZLTextPositionIndicatorInfo> indicatorInfo = this->indicatorInfo();
	if (!indicatorInfo.isNull() &&
			(indicatorInfo->type() == ZLTextPositionIndicatorInfo::FB_INDICATOR) &&
			indicatorInfo->isSensitive() &&
			positionIndicator()->isResponsibleFor(x, y)) {
		return true;
	}

	if (myDoubleClickInfo.Count > 0) {
		return onStylusClick(x, y, myDoubleClickInfo.Count);
	}

	myTextAreaController.area().selectionModel().deactivate();
	return false;
}

void ZLTextView::clearCaches() {
	myTextAreaController.rebuildPaintInfo(true);
}

void ZLTextView::highlightParagraph(int paragraphIndex) {
	textArea().model()->selectParagraph(paragraphIndex);
	myTextAreaController.rebuildPaintInfo(true);
}

void ZLTextView::gotoCharIndex(size_t charIndex) {
	shared_ptr<ZLTextModel> model = textArea().model();
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
		size_t endCharIndex = positionIndicator()->sizeOfTextBeforeCursor(textArea().endCursor());
		if (endCharIndex > charIndex) {
			while (endCharIndex > charIndex) {
				scrollPage(false, ZLTextAreaController::SCROLL_LINES, 1);
				preparePaintInfo();
				if (positionIndicator()->sizeOfTextBeforeCursor(textArea().startCursor()) <= myTextSize[startParagraphIndex]) {
					break;
				}
				endCharIndex = positionIndicator()->sizeOfTextBeforeCursor(textArea().endCursor());
			}
			if (endCharIndex < charIndex) {
				scrollPage(true, ZLTextAreaController::SCROLL_LINES, 1);
			}
		} else {
			int startCharIndex = positionIndicator()->sizeOfTextBeforeCursor(textArea().startCursor());
			while (endCharIndex < charIndex) {
				scrollPage(true, ZLTextAreaController::SCROLL_LINES, 1);
				preparePaintInfo();
				const int newStartCharIndex = positionIndicator()->sizeOfTextBeforeCursor(textArea().startCursor());
				if (newStartCharIndex <= startCharIndex) {
					break;
				}
				startCharIndex = newStartCharIndex;
				endCharIndex = positionIndicator()->sizeOfTextBeforeCursor(textArea().endCursor());
			}
			if (endCharIndex > charIndex) {
				scrollPage(false, ZLTextAreaController::SCROLL_LINES, 1);
			}
		}
	}
}

void ZLTextView::gotoPage(size_t index) {
	size_t charIndex = (index - 1) * 2048;
	std::vector<size_t>::const_iterator it = std::lower_bound(myTextSize.begin(), myTextSize.end(), charIndex);
	const int paraIndex = it - myTextSize.begin();
	const ZLTextParagraph &para = *(*textArea().model())[paraIndex];
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
	if (textArea().isEmpty() || positionIndicator().isNull() || textArea().endCursor().isNull()) {
		return 0;
	}
	return positionIndicator()->sizeOfTextBeforeCursor(textArea().endCursor()) / 2048 + 1;
}

size_t ZLTextView::pageNumber() const {
	if (textArea().isEmpty()) {
		return 0;
	}
	std::vector<size_t>::const_iterator i = nextBreakIterator();
	const size_t startIndex = (i != myTextBreaks.begin()) ? *(i - 1) : 0;
	const size_t endIndex = (i != myTextBreaks.end()) ? *i : textArea().model()->paragraphsNumber();
	return (myTextSize[endIndex] - myTextSize[startIndex]) / 2048 + 1;
}

void ZLTextView::onScrollbarMoved(Direction direction, size_t full, size_t from, size_t to) {
	if (direction != VERTICAL) {
		return;
	}

	myTextAreaController.area().selectionModel().deactivate();

  if (textArea().model().isNull()) {
	  return;
	}

	if (textArea().startCursor().isNull() || textArea().endCursor().isNull()) {
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
		scrollPage(forward, ZLTextAreaController::SCROLL_LINES, forward ? steps : -steps);
		ZLApplication::Instance().refreshWindow();
	}
}

void ZLTextView::onScrollbarPageStep(Direction direction, int steps) {
	if (direction == VERTICAL) {
		const bool forward = steps > 0;
		scrollPage(forward, ZLTextAreaController::NO_OVERLAPPING, forward ? steps : -steps);
		ZLApplication::Instance().refreshWindow();
	}
}

void ZLTextView::forceScrollbarUpdate() {
	myDoUpdateScrollbar = true;
}

ZLTextView::DoubleClickInfo::DoubleClickInfo(const ZLTextView &view) : myView(view) {
	Count = 0;
}

void ZLTextView::DoubleClickInfo::update(int x, int y, bool press) {
	ZLTime current;
	int dcDeltaX = X - x;
	int dcDeltaY = Y - y;
	if ((current.millisecondsFrom(Time) < myView.doubleClickDelay()) &&
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

ZLTextSelectionModel &ZLTextView::selectionModel() {
	return myTextAreaController.area().selectionModel();
}

void ZLTextView::startSelectionScrolling(bool forward) {
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
			ZLTimeManager::Instance().removeTask(mySelectionScroller);
		}
		((ZLTextSelectionScroller&)*mySelectionScroller).setDirection(direction);
		ZLTimeManager::Instance().addTask(mySelectionScroller, 400);
	}
}

void ZLTextView::stopSelectionScrolling() {
	if (!mySelectionScroller.isNull()) {
		((ZLTextSelectionScroller&)*mySelectionScroller).setDirection(ZLTextSelectionScroller::DONT_SCROLL);
		ZLTimeManager::Instance().removeTask(mySelectionScroller);
	}
}

void ZLTextView::scrollPage(bool forward, ZLTextAreaController::ScrollingMode mode, unsigned int value) {
	preparePaintInfo();
	myTextAreaController.scrollPage(forward, mode, value);
}

void ZLTextView::preparePaintInfo() {
	size_t newWidth = 
		std::max(context().width() - leftMargin() - rightMargin(), 1);
	int viewHeight = context().height() - topMargin() - bottomMargin();
	shared_ptr<ZLTextPositionIndicatorInfo> indicatorInfo = this->indicatorInfo();
	if (!indicatorInfo.isNull() && (indicatorInfo->type() == ZLTextPositionIndicatorInfo::FB_INDICATOR)) {
		viewHeight -= indicatorInfo->height() + indicatorInfo->offset();
	}
	size_t newHeight = std::max(viewHeight, 1);

	if (newWidth != myTextAreaController.area().width() || newHeight != myTextAreaController.area().height()) {
		myTextAreaController.area().setSize(newWidth, newHeight);
		myTextAreaController.rebuildPaintInfo(false);
	}

	if (myTextAreaController.preparePaintInfo()) {
		myDoUpdateScrollbar = true;
	}
}
