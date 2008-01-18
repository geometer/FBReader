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

#include <ZLOptions.h>
#include <ZLDialogManager.h>
#include <ZLStringUtil.h>

#include <ZLTextModel.h>
#include <ZLTextParagraph.h>
#include <ZLTextParagraphCursor.h>
#include <ZLTextWord.h>

#include "BookTextView.h"
#include "FBReader.h"

#include "../bookmodel/FBTextKind.h"
#include "../bookmodel/BookModel.h"
#include "../external/ProgramCollection.h"

static const std::string PARAGRAPH_OPTION_NAME = "Paragraph";
static const std::string WORD_OPTION_NAME = "Word";
static const std::string CHAR_OPTION_NAME = "Char";
static const std::string BUFFER_SIZE = "UndoBufferSize";
static const std::string POSITION_IN_BUFFER = "PositionInBuffer";
static const char * const BUFFER_PARAGRAPH_PREFIX = "Paragraph_";
static const char * const BUFFER_WORD_PREFIX = "Word_";

BookTextView::BookTextView(FBReader &reader, shared_ptr<ZLPaintContext> context) :
	FBView(reader, context),
	ShowTOCMarksOption(ZLCategoryKey::LOOK_AND_FEEL, "Indicator", "ShowTOCMarks", false) {
	myCurrentPointInStack = 0;
	myMaxStackSize = 20;
	myLockUndoStackChanges = false;
}

BookTextView::~BookTextView() {
	saveState();
}

void BookTextView::setModel(shared_ptr<ZLTextModel> model, const std::string &fileName) {
	FBView::setModel(model);

	myFileName = fileName;

	gotoPosition(
		ZLIntegerOption(ZLCategoryKey::STATE, fileName, PARAGRAPH_OPTION_NAME, 0).value(),
		ZLIntegerOption(ZLCategoryKey::STATE, fileName, WORD_OPTION_NAME, 0).value(),
		ZLIntegerOption(ZLCategoryKey::STATE, fileName, CHAR_OPTION_NAME, 0).value()
	);

	myPositionStack.clear();
	myCurrentPointInStack = 0;

	int stackSize = ZLIntegerOption(ZLCategoryKey::STATE, fileName, BUFFER_SIZE, 0).value();
	if (stackSize > 0) {
		if (stackSize > (int)myMaxStackSize) {
			stackSize = myMaxStackSize;
		}
		int pointInStack = ZLIntegerOption(ZLCategoryKey::STATE, fileName, POSITION_IN_BUFFER, 0).value();
		if ((pointInStack < 0) || (pointInStack > stackSize)) {
			pointInStack = stackSize;
		}
		myCurrentPointInStack = pointInStack;

		for (int i = 0; i < stackSize; ++i) {
			std::string bufferParagraph = BUFFER_PARAGRAPH_PREFIX;
			std::string bufferWord = BUFFER_WORD_PREFIX;
			ZLStringUtil::appendNumber(bufferParagraph, i);
			ZLStringUtil::appendNumber(bufferWord, i);
			Position pos;
			pos.first = ZLIntegerOption(ZLCategoryKey::STATE, fileName, bufferParagraph, -1).value();
			pos.second = ZLIntegerOption(ZLCategoryKey::STATE, fileName, bufferWord, -1).value();
			myPositionStack.push_back(pos);
		}
	}
}

void BookTextView::setContentsModel(shared_ptr<ZLTextModel> contentsModel) {
	myContentsModel = contentsModel;
}

void BookTextView::saveState() {
	const ZLTextWordCursor &cursor = startCursor();

	if (!cursor.isNull()) {
		ZLIntegerOption(ZLCategoryKey::STATE, myFileName, PARAGRAPH_OPTION_NAME, 0).setValue(cursor.paragraphCursor().index());
		ZLIntegerOption(ZLCategoryKey::STATE, myFileName, WORD_OPTION_NAME, 0).setValue(cursor.wordNumber());
		ZLIntegerOption(ZLCategoryKey::STATE, myFileName, CHAR_OPTION_NAME, 0).setValue(cursor.charNumber());
		ZLIntegerOption(ZLCategoryKey::STATE, myFileName, BUFFER_SIZE, 0).setValue(myPositionStack.size());
		ZLIntegerOption(ZLCategoryKey::STATE, myFileName, POSITION_IN_BUFFER, 0).setValue(myCurrentPointInStack);

		for (unsigned int i = 0; i < myPositionStack.size(); ++i) {
			std::string bufferParagraph = BUFFER_PARAGRAPH_PREFIX;
			std::string bufferWord = BUFFER_WORD_PREFIX;
			ZLStringUtil::appendNumber(bufferParagraph, i);
			ZLStringUtil::appendNumber(bufferWord, i);
			ZLIntegerOption(ZLCategoryKey::STATE, myFileName, bufferParagraph, -1).setValue(myPositionStack[i].first);
			ZLIntegerOption(ZLCategoryKey::STATE, myFileName, bufferWord, -1).setValue(myPositionStack[i].second);
		}
	}
}

BookTextView::Position BookTextView::cursorPosition(const ZLTextWordCursor &cursor) const {
	return Position(cursor.paragraphCursor().index(), cursor.wordNumber());
}

bool BookTextView::pushCurrentPositionIntoStack(bool doPushSamePosition) {
	const ZLTextWordCursor &cursor = startCursor();
	if (cursor.isNull()) {
		return false;
	}

	Position pos = cursorPosition(cursor);
	if (!doPushSamePosition && !myPositionStack.empty() && (myPositionStack.back() == pos)) {
		return false;
	}

	myPositionStack.push_back(pos);
	while (myPositionStack.size() > myMaxStackSize) {
		myPositionStack.erase(myPositionStack.begin());
		if (myCurrentPointInStack > 0) {
			--myCurrentPointInStack;
		}
	}
	return true;
}

void BookTextView::replaceCurrentPositionInStack() {
	const ZLTextWordCursor &cursor = startCursor();
	if (!cursor.isNull()) {
		myPositionStack[myCurrentPointInStack] = cursorPosition(cursor);
	}
}

void BookTextView::gotoParagraph(int num, bool last) {
	if (!empty()) {
		if (!myLockUndoStackChanges) {
			if (myPositionStack.size() > myCurrentPointInStack) {
				myPositionStack.erase(myPositionStack.begin() + myCurrentPointInStack, myPositionStack.end());
			}
			pushCurrentPositionIntoStack(false);
			myCurrentPointInStack = myPositionStack.size();
		}

		FBView::gotoParagraph(num, last);
	}
}

bool BookTextView::canUndoPageMove() {
	if (empty()) {
		return false;
	}
	if (myCurrentPointInStack == 0) {
		return false;
	}
	if ((myCurrentPointInStack == 1) && (myPositionStack.size() == 1)) {
		const ZLTextWordCursor &cursor = startCursor();
		if (!cursor.isNull()) {
			return myPositionStack.back() != cursorPosition(cursor);
		}
	}
	return true;
}

void BookTextView::undoPageMove() {
	if (canUndoPageMove()) {
		if (myCurrentPointInStack == myPositionStack.size()) {
			if (!pushCurrentPositionIntoStack(false)) {
				-- myCurrentPointInStack;
			}
		} else {
			replaceCurrentPositionInStack();
		}

		--myCurrentPointInStack;
		Position &pos = myPositionStack[myCurrentPointInStack];
		myLockUndoStackChanges = true;
		gotoPosition(pos.first, pos.second, 0);
		myLockUndoStackChanges = false;

		application().refreshWindow();
	}
}

bool BookTextView::canRedoPageMove() {
	return !empty() && (myCurrentPointInStack + 1 < myPositionStack.size());
}

void BookTextView::redoPageMove() {
	if (canRedoPageMove()) {
		replaceCurrentPositionInStack();
		++myCurrentPointInStack;
		Position &pos = myPositionStack[myCurrentPointInStack];
		myLockUndoStackChanges = true;
		gotoPosition(pos.first, pos.second, 0);
		myLockUndoStackChanges = false;

		if (myCurrentPointInStack + 1 == myPositionStack.size()) {
			myPositionStack.pop_back();
		}

		application().refreshWindow();
	}
}

bool BookTextView::getHyperlinkId(const ZLTextElementArea &area, std::string &id, bool &isExternal) const {
	if ((area.Kind != ZLTextElement::WORD_ELEMENT) &&
			(area.Kind != ZLTextElement::IMAGE_ELEMENT)) {
		return false;
	}
	ZLTextWordCursor cursor = startCursor();
	cursor.moveToParagraph(area.ParagraphNumber);
	cursor.moveToParagraphStart();
	ZLTextKind hyperlinkKind = REGULAR;
	for (int i = 0; i < area.TextElementNumber; ++i) {
		const ZLTextElement &element = cursor.element();
		if (element.kind() == ZLTextElement::CONTROL_ELEMENT) {
			const ZLTextControlEntry &control = ((const ZLTextControlElement&)element).entry();
			if (control.isHyperlink()) {
				hyperlinkKind = control.kind();
				id = ((const ZLTextHyperlinkControlEntry&)control).label();
			} else if (!control.isStart() && (control.kind() == hyperlinkKind)) {
				hyperlinkKind = REGULAR;
			}
		}
		cursor.nextWord();
	}

	isExternal = hyperlinkKind == EXTERNAL_HYPERLINK;
	return hyperlinkKind != REGULAR;
}

bool BookTextView::_onStylusPress(int x, int y) {
	const ZLTextElementArea *area = elementByCoordinates(x, y);
	if (area != 0) {
		std::string id;
		bool isExternal;
		if (getHyperlinkId(*area, id, isExternal)) {
			fbreader().tryShowFootnoteView(id, isExternal);
			return true;
		}
		
		if (fbreader().isDictionarySupported() &&
				fbreader().EnableSingleClickDictionaryOption.value()) {
			const std::string txt = word(*area);
			if (!txt.empty()) {
				fbreader().openInDictionary(txt);
				return true;
			}
		}
	}

	return false;
}

bool BookTextView::onStylusMove(int x, int y) {
	const ZLTextElementArea *area = elementByCoordinates(x, y);
	std::string id;
	bool isExternal;
	fbreader().setHyperlinkCursor((area != 0) && getHyperlinkId(*area, id, isExternal));
	return true;
}

shared_ptr<ZLTextView::PositionIndicator> BookTextView::createPositionIndicator(const ZLTextPositionIndicatorInfo &info) {
	return new PositionIndicatorWithLabels(*this, info);
}

BookTextView::PositionIndicatorWithLabels::PositionIndicatorWithLabels(BookTextView &bookTextView, const ZLTextPositionIndicatorInfo &info) : PositionIndicator(bookTextView, info) {
}

void BookTextView::PositionIndicatorWithLabels::draw() {
	PositionIndicator::draw();

	const BookTextView& bookTextView = (const BookTextView&)textView();

	if (bookTextView.ShowTOCMarksOption.value()) {
		shared_ptr<ZLTextModel> contentsModelPtr = bookTextView.myContentsModel;
		if (!contentsModelPtr.isNull()) {
			ContentsModel &contentsModel = (ContentsModel&)*contentsModelPtr;
			const int marksNumber = contentsModel.paragraphsNumber();
			const size_t startIndex = startTextIndex();
			const size_t endIndex = endTextIndex();
			const std::vector<size_t> &textSizeVector = textSize();
			const int fullWidth = right() - left() - 1;
			const size_t startPosition = textSizeVector[startIndex];
			const size_t fullTextSize = textSizeVector[endIndex] - startPosition;
			const int bottom = this->bottom();
			const int top = this->top();
			for (int i = 0; i < marksNumber; ++i) {
				size_t reference = contentsModel.reference((ZLTextTreeParagraph*)contentsModel[i]);
				if ((startIndex < reference) && (reference < endIndex)) {
					int position = left() + 2 + (int)
						(1.0 * fullWidth * (textSizeVector[reference] - startPosition) / fullTextSize);
					context().drawLine(position, bottom, position, top);
				}
			}
		}
	}
}

void BookTextView::scrollToHome() {
	if (!startCursor().isNull() &&
			startCursor().isStartOfParagraph() &&
			startCursor().paragraphCursor().index() == 0) {
		return;
	}

	gotoParagraph(0, false);
	fbreader().refreshWindow();
}
