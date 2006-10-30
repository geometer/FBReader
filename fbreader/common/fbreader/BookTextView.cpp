/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <ZLOptions.h>
#include <ZLDialogManager.h>
#include <ZLStringUtil.h>
#include <ZLUnicodeUtil.h>
#include <ZLDictionary.h>

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

BookTextView::BookTextView(FBReader &reader, ZLPaintContext &context) : FBView(reader, context) {
	myCurrentPointInStack = 0;
	myMaxStackSize = 20;
}

BookTextView::~BookTextView() {
	saveState();
}

void BookTextView::setModel(const TextModel *model, const std::string &name) {
	TextView::setModel(model, name);

	const std::string &group = fileName();

	gotoPosition(
		ZLIntegerOption(ZLOption::STATE_CATEGORY, group, PARAGRAPH_OPTION_NAME, 0).value(),
		ZLIntegerOption(ZLOption::STATE_CATEGORY, group, WORD_OPTION_NAME, 0).value(),
		ZLIntegerOption(ZLOption::STATE_CATEGORY, group, CHAR_OPTION_NAME, 0).value()
	);

	myPositionStack.clear();
	myCurrentPointInStack = 0;

	int stackSize = ZLIntegerOption(ZLOption::STATE_CATEGORY, group, BUFFER_SIZE, 0).value();
	if (stackSize > 0) {
		if (stackSize > (int)myMaxStackSize) {
			stackSize = myMaxStackSize;
		}
		int pointInStack = ZLIntegerOption(ZLOption::STATE_CATEGORY, group, POSITION_IN_BUFFER, 0).value();
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
			pos.first = ZLIntegerOption(ZLOption::STATE_CATEGORY, group, bufferParagraph, -1).value();
			pos.second = ZLIntegerOption(ZLOption::STATE_CATEGORY, group, bufferWord, -1).value();
			myPositionStack.push_back(pos);
		}
	}
}

void BookTextView::saveState() {
	const WordCursor &cursor = startCursor();
	const std::string &group = fileName();

	if (!cursor.isNull()) {
		ZLIntegerOption(ZLOption::STATE_CATEGORY, group, PARAGRAPH_OPTION_NAME, 0).setValue(cursor.paragraphCursor().index());
		ZLIntegerOption(ZLOption::STATE_CATEGORY, group, WORD_OPTION_NAME, 0).setValue(cursor.wordNumber());
		ZLIntegerOption(ZLOption::STATE_CATEGORY, group, CHAR_OPTION_NAME, 0).setValue(cursor.charNumber());
		ZLIntegerOption(ZLOption::STATE_CATEGORY, group, BUFFER_SIZE, 0).setValue(myPositionStack.size());
		ZLIntegerOption(ZLOption::STATE_CATEGORY, group, POSITION_IN_BUFFER, 0).setValue(myCurrentPointInStack);

		for (unsigned int i = 0; i < myPositionStack.size(); ++i) {
			std::string bufferParagraph = BUFFER_PARAGRAPH_PREFIX;
			std::string bufferWord = BUFFER_WORD_PREFIX;
			ZLStringUtil::appendNumber(bufferParagraph, i);
			ZLStringUtil::appendNumber(bufferWord, i);
			ZLIntegerOption(ZLOption::STATE_CATEGORY, group, bufferParagraph, -1).setValue(myPositionStack[i].first);
			ZLIntegerOption(ZLOption::STATE_CATEGORY, group, bufferWord, -1).setValue(myPositionStack[i].second);
		}
	}
}

void BookTextView::pushCurrentPositionIntoStack() {
	const WordCursor &cursor = startCursor();
	Position pos;
	pos.first = cursor.paragraphCursor().index();
	pos.second = cursor.wordNumber();
	myPositionStack.push_back(pos);
	while (myPositionStack.size() > myMaxStackSize) {
		myPositionStack.erase(myPositionStack.begin());
		if (myCurrentPointInStack > 0) {
			--myCurrentPointInStack;
		}
	}
}

void BookTextView::replaceCurrentPositionInStack() {
	const WordCursor &cursor = startCursor();
	myPositionStack[myCurrentPointInStack].first = cursor.paragraphCursor().index();
	myPositionStack[myCurrentPointInStack].second = cursor.wordNumber();
}

void BookTextView::gotoParagraph(int num, bool last) {
	if (!empty()) {
		while (myPositionStack.size() > myCurrentPointInStack) {
			myPositionStack.pop_back();
		}
		pushCurrentPositionIntoStack();
		++myCurrentPointInStack;

		TextView::gotoParagraph(num, last);
	}
}

bool BookTextView::canUndoPageMove() {
	return !empty() && (myCurrentPointInStack > 0) && (myCurrentPointInStack <= myPositionStack.size());
}

void BookTextView::undoPageMove() {
	if (canUndoPageMove()) {
		if (myCurrentPointInStack == myPositionStack.size()) {
			pushCurrentPositionIntoStack();
		} else {
			replaceCurrentPositionInStack();
		}

		--myCurrentPointInStack;
		Position &pos = myPositionStack[myCurrentPointInStack];
		gotoPosition(pos.first, pos.second, 0);

		repaintView();
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
		gotoPosition(pos.first, pos.second, 0);

		if (myCurrentPointInStack + 1 == myPositionStack.size()) {
			myPositionStack.pop_back();
		}

		repaintView();
	}
}

bool BookTextView::onStylusPress(int x, int y) {
	if (TextView::onStylusPress(x, y)) {
		return true;
	}
	const TextElementPosition *position = elementByCoordinates(x, y);
	if ((position == 0) ||
			((position->Kind != TextElement::WORD_ELEMENT) && (position->Kind != TextElement::IMAGE_ELEMENT))) {
		return false;
	}
	WordCursor cursor = startCursor();
	cursor.moveToParagraph(position->ParagraphNumber);
	cursor.moveToParagraphStart();
	bool isHyperlink = false;
	std::string id;
	TextKind hyperlinkKind = REGULAR;
	for (int i = 0; i < position->TextElementNumber; ++i) {
		const TextElement &element = cursor.element();
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
		cursor.nextWord();
	}

	if (isHyperlink) {
		fbreader().tryShowFootnoteView(id);
		return true;
	}
	
	if (fbreader().EnableDictionaryIntegrationOption.value() &&
			(position->Kind == TextElement::WORD_ELEMENT)) {
		ZLDictionary &dictionary = ZLDictionary::instance();
		if (dictionary.isDictionaryEnabled()) {
			const Word &word = (Word&)cursor.element();
			ZLUnicodeUtil::Ucs2String ucs2;
			ZLUnicodeUtil::utf8ToUcs2(ucs2, word.Data, word.Size);
			ZLUnicodeUtil::Ucs2String::iterator it = ucs2.begin();
			while ((it != ucs2.end()) && !ZLUnicodeUtil::isLetter(*it)) {
				++it;
			}
			if (it != ucs2.end()) {
				ucs2.erase(ucs2.begin(), it);
				it = ucs2.end() - 1;
				while (!ZLUnicodeUtil::isLetter(*it)) {
					--it;
				}
				ucs2.erase(it + 1, ucs2.end());
    
				std::string txt;
				ZLUnicodeUtil::ucs2ToUtf8(txt, ucs2);
				dictionary.openInDictionary(txt);
				return true;
			}
		}
	}
	return false;
}
