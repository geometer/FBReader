/*
 * FBReader -- electronic book reader
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <abstract/ZLOptions.h>
#include <abstract/ZLDialogManager.h>

#include "BookTextView.h"
#include "FBReader.h"

#include "../textview/ParagraphCursor.h"
#include "../textview/Word.h"

#include "../model/TextModel.h"
#include "../model/Paragraph.h"

static const std::string PARAGRAPH_OPTION_NAME = "Paragraph";
static const std::string WORD_OPTION_NAME = "Word";
static const std::string CHAR_OPTION_NAME = "Char";
static const std::string BUFFER_SIZE = "UndoBufferSize";
static const std::string POSITION_IN_BUFFER = "PositionInBuffer";
static const char * const BUFFER_PARAGRAPH_PREFIX = "Paragraph_";
static const char * const BUFFER_WORD_PREFIX = "Word_";

BookTextView::BookTextView(FBReader &reader, PaintContext &context) : TextView(context), myReader(reader) {
	myCurrentPointInStack = 0;
	myMaxStackSize = 20;
}

BookTextView::~BookTextView() {
}

void BookTextView::setModel(const TextModel *model, const std::string &name) {
	TextView::setModel(model, name);

	if ((myModel != 0) && !myModel->paragraphs().empty()) {
		ZLIntegerOption paragraphPosition(myName, PARAGRAPH_OPTION_NAME, 0);
		ZLIntegerOption wordPosition(myName, WORD_OPTION_NAME, 0);
		ZLIntegerOption charPosition(myName, CHAR_OPTION_NAME, 0);
		myFirstParagraphCursor->moveTo(paragraphPosition.value(), wordPosition.value(), charPosition.value());
	}

	myPositionStack.clear();
	myCurrentPointInStack = 0;

	int stackSize = ZLIntegerOption(myName, BUFFER_SIZE, 0).value();
	if (stackSize > 0) {
		if (stackSize > (int)myMaxStackSize) {
			stackSize = myMaxStackSize;
		}
		int pointInStack = ZLIntegerOption(myName, POSITION_IN_BUFFER, 0).value();
		if ((pointInStack < 0) || (pointInStack > stackSize)) {
			pointInStack = stackSize;
		}
		myCurrentPointInStack = pointInStack;

		for (int i = 0; i < stackSize; i++) {
			std::string bufferParagraph = BUFFER_PARAGRAPH_PREFIX;
			std::string bufferWord = BUFFER_WORD_PREFIX;
			ZLStringUtil::appendNumber(bufferParagraph, i);
			ZLStringUtil::appendNumber(bufferWord, i);
			std::pair<int,int> pos;
			pos.first = ZLIntegerOption(myName, bufferParagraph, -1).value();
			pos.second = ZLIntegerOption(myName, bufferWord, -1).value();
			myPositionStack.push_back(pos);
		}
	}
}

void BookTextView::saveState() {
	if ((myModel == 0) || (myFirstParagraphCursor == 0)) {
		return;
	}

	ZLIntegerOption(myName, PARAGRAPH_OPTION_NAME, 0).setValue(myFirstParagraphCursor->paragraphNumber());
	ZLIntegerOption(myName, WORD_OPTION_NAME, 0).setValue(myFirstParagraphCursor->wordNumber());
	ZLIntegerOption(myName, CHAR_OPTION_NAME, 0).setValue(myFirstParagraphCursor->charNumber());
	ZLIntegerOption(myName, BUFFER_SIZE, 0).setValue(myPositionStack.size());
	ZLIntegerOption(myName, POSITION_IN_BUFFER, 0).setValue(myCurrentPointInStack);

	for (unsigned int i = 0; i < myPositionStack.size(); i++) {
		std::string bufferParagraph = BUFFER_PARAGRAPH_PREFIX;
		std::string bufferWord = BUFFER_WORD_PREFIX;
		ZLStringUtil::appendNumber(bufferParagraph, i);
		ZLStringUtil::appendNumber(bufferWord, i);
		ZLIntegerOption(myName, bufferParagraph, -1).setValue(myPositionStack[i].first);
		ZLIntegerOption(myName, bufferWord, -1).setValue(myPositionStack[i].second);
	}
}

void BookTextView::pushCurrentPositionIntoStack() {
	std::pair<int,int> pos;
	pos.first = myFirstParagraphCursor->paragraphNumber();
	pos.second = myFirstParagraphCursor->wordNumber();
	myPositionStack.push_back(pos);
	while (myPositionStack.size() > myMaxStackSize) {
#ifndef PALM_TEMPORARY
		myPositionStack.erase(myPositionStack.begin());
#endif // PALM_TEMPORARY
		if (myCurrentPointInStack > 0) {
			myCurrentPointInStack--;
		}
	}
}

void BookTextView::replaceCurrentPositionInStack() {
	myPositionStack[myCurrentPointInStack].first = myFirstParagraphCursor->paragraphNumber();
	myPositionStack[myCurrentPointInStack].second = myFirstParagraphCursor->wordNumber();
}

bool BookTextView::setFirstParagraphCursor() {
	if (myFirstParagraphCursor == 0) {
		if (myLastParagraphCursor == 0) {
			return false;
		}
		myFirstParagraphCursor = myLastParagraphCursor;
		myLastParagraphCursor = 0;
	}
	if (myLastParagraphCursor != 0) {
		delete myLastParagraphCursor;
		myLastParagraphCursor = 0;
	}
	return true;
}

void BookTextView::gotoParagraph(int num, bool last) {
	if (!setFirstParagraphCursor()) {
		return;
	}

	while (myPositionStack.size() > myCurrentPointInStack) {
		myPositionStack.pop_back();
	}
	pushCurrentPositionIntoStack();
	myCurrentPointInStack++;

	TextView::gotoParagraph(num, last);
}

bool BookTextView::canUndoPageMove() {
	return (myCurrentPointInStack > 0) && (myCurrentPointInStack <= myPositionStack.size());
}

void BookTextView::undoPageMove() {
	if (!canUndoPageMove() || !setFirstParagraphCursor()) {
		return;
	}

	if (myCurrentPointInStack == myPositionStack.size()) {
		pushCurrentPositionIntoStack();
	} else {
		replaceCurrentPositionInStack();
	}

	myCurrentPointInStack--;
	std::pair<int,int> &pos = myPositionStack[myCurrentPointInStack];
	myFirstParagraphCursor->moveTo(pos.first, pos.second, 0);

	repaintView();
}

bool BookTextView::canRedoPageMove() {
	return myCurrentPointInStack + 1 < myPositionStack.size();
}

void BookTextView::redoPageMove() {
	if (!canRedoPageMove() || !setFirstParagraphCursor()) {
		return;
	}

	replaceCurrentPositionInStack();
	myCurrentPointInStack++;
	std::pair<int,int> &pos = myPositionStack[myCurrentPointInStack];
	myFirstParagraphCursor->moveTo(pos.first, pos.second, 0);

	if (myCurrentPointInStack + 1 == myPositionStack.size()) {
		myPositionStack.pop_back();
	}

	repaintView();
}

bool BookTextView::onStylusPress(int x, int y) {
	if (TextView::onStylusPress(x, y)) {
		return true;
	}
	const TextElementPosition *position = elementByCoordinates(x, y);
	if ((position == 0) || (position->Kind != TextElement::WORD_ELEMENT)) {
		return false;
	}
	ParagraphCursor *cursor = myFirstParagraphCursor->createCopy();
	cursor->moveTo(position->ParagraphNumber, 0, 0);
	bool isHyperlink = false;
	std::string id;
	TextKind hyperlinkKind = REGULAR;
	WordCursor wordCursor = cursor->begin();
	for (int i = 0; i < position->TextElementNumber; i++) {
		const TextElement &element = wordCursor.element();
		if (element.kind() == TextElement::CONTROL_ELEMENT) {
			const ControlEntry &control = ((const ControlElement&)element).entry();
			if (control.isHyperlink()) {
				isHyperlink = true;
				hyperlinkKind = control.kind();
				id = ((const HyperlinkControlEntry&)control).label();
			} else if (isHyperlink && !control.isStart() && (control.kind() == hyperlinkKind)) {
				isHyperlink = false;
			}
		}
		wordCursor.nextWord();
	}

	if (isHyperlink) {
		myReader.tryShowFootnoteView(id);
	}
	delete cursor;
	return isHyperlink;
}
