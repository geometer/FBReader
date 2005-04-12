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

#include <iostream>

#include "TextView.h"
#include "ParagraphCursor.h"
#include "TextStyle.h"
#include "Word.h"

#include "../model/TextModel.h"
#include "../model/Paragraph.h"

#include "../view/PaintContext.h"

ZLColorOption TextView::TreeLinesColorOption("Colors", "TreeLines", ZLColor(127, 127, 127));
ZLBooleanOption TextView::AutoHyphenationOption("Options", "AutoHyphenation", true);

ZLBooleanOption TextView::ShowPositionIndicatorOption("Indicator", "Show", true);
ZLColorOption TextView::PositionIndicatorColorOption("Indicator", "Color", ZLColor(127, 127, 127));
ZLIntegerOption TextView::PositionIndicatorHeightOption("Indicator", "Height", 16);
ZLIntegerOption TextView::PositionIndicatorOffsetOption("Indicator", "Offset", 4);

TextView::TextView(PaintContext &context) : View(context), myStyle(context), myLineProcessor(myStyle) {
	myModel = NULL;

	myFirstParagraphCursor = NULL;
	myLastParagraphCursor = NULL;
}

TextView::~TextView() {
	clear();
}

void TextView::clear() {
	if (myFirstParagraphCursor != 0) {
		delete myFirstParagraphCursor;
		myFirstParagraphCursor = 0;
	}
	if (myLastParagraphCursor != 0) {
		delete myLastParagraphCursor;
		myLastParagraphCursor = 0;
	}
	myParagraphMap.clear();
	myTextElementMap.clear();
}

void TextView::setModel(const TextModel *model, const std::string &name) {
	clear();

	myModel = model;

	if ((myModel != 0) && !myModel->paragraphs().empty()) {
		myFirstParagraphCursor = ParagraphCursor::createCursor(*myModel);
		myName = name;
	}
}

void TextView::paint(bool doPaint) {
	myParagraphMap.clear();
	myTextElementMap.clear();

	context().clear();

	if (myFirstParagraphCursor == NULL) {
		if (myLastParagraphCursor == NULL) {
			return;
		}
		myFirstParagraphCursor = myLastParagraphCursor;
		myLastParagraphCursor = NULL;
		int height = paragraphHeight(*myFirstParagraphCursor, true);
		bool positionChanged = !myFirstParagraphCursor->isStartOfParagraph();
		myFirstParagraphCursor->moveToParagraphStart();
		while (height < myStyle.textAreaHeight()) {
			if (positionChanged && myFirstParagraphCursor->isEndOfSection()) {
				break;
			}
			if (!myFirstParagraphCursor->previous()) {
				break;
			}
			if (!myFirstParagraphCursor->isEndOfSection()) {
				positionChanged = true;
			}
			height += paragraphHeight(*myFirstParagraphCursor, false);
		}
		skip(*myFirstParagraphCursor, height - myStyle.textAreaHeight());
	}

	if (myLastParagraphCursor != NULL) {
		delete myLastParagraphCursor;
	}
	myLastParagraphCursor = myFirstParagraphCursor->createCopy();

	context().moveYTo(0);
	do {
		int start = context().y() + 1;
		drawParagraph(*myLastParagraphCursor, doPaint);
		int end = context().y();
		myParagraphMap.push_back(ParagraphPosition(myLastParagraphCursor->paragraphNumber(), start, end));
	} while (myLastParagraphCursor->isEndOfParagraph() && myLastParagraphCursor->next() && !myLastParagraphCursor->isEndOfSection());

	if (doPaint && ShowPositionIndicatorOption.value()) {
		int bottom = context().height();
		int top = bottom - PositionIndicatorHeightOption.value() + 1;
		int left = 0;
		int right = context().width() - 1;
		int fillWidth;
		int paragraphLength = myLastParagraphCursor->paragraphLength();
		if (paragraphLength == 0) {
			fillWidth = (right - left - 1) * (myLastParagraphCursor->paragraphNumber() + 1) / myModel->paragraphs().size();
		} else {
			fillWidth = (right - left - 1) * (myLastParagraphCursor->paragraphNumber() * paragraphLength + myLastParagraphCursor->wordNumber()) / myModel->paragraphs().size() / paragraphLength;
		}
		context().setFillColor(PositionIndicatorColorOption.value());
		context().fillRectangle(left + 1, top + 1, left + fillWidth + 1, bottom - 1);
		context().drawLine(left, top, right, top);
		context().drawLine(left, bottom, right, bottom);
		context().drawLine(left, bottom, left, top);
		context().drawLine(right, bottom, right, top);
	}
}

void TextView::scrollPageBackward() {
	if ((myFirstParagraphCursor != NULL) && !myFirstParagraphCursor->isStartOfText()) {
		if (myLastParagraphCursor != NULL) {
			delete myLastParagraphCursor;
		}
		myLastParagraphCursor = myFirstParagraphCursor;
		myFirstParagraphCursor = NULL;
	}
}

void TextView::scrollPageForward() {
	if ((myLastParagraphCursor != NULL) && !myLastParagraphCursor->isEndOfText()) {
		if (myFirstParagraphCursor != NULL) {
			delete myFirstParagraphCursor;
		}
		myFirstParagraphCursor = myLastParagraphCursor;
		myLastParagraphCursor = NULL;
	}
}

const TextView::ParagraphPosition *TextView::paragraphByCoordinate(int y) const {
	for (std::vector<ParagraphPosition>::const_iterator it = myParagraphMap.begin(); it != myParagraphMap.end(); it++) {
		const ParagraphPosition &position = *it;
		if ((position.YStart <= y) && (y <= position.YEnd)) {
			return &position;
		}
	}
	return 0;
}

const TextView::TextElementPosition *TextView::elementByCoordinates(int x, int y) const {
	for (std::vector<TextElementPosition>::const_iterator it = myTextElementMap.begin(); it != myTextElementMap.end(); it++) {
		const TextElementPosition &position = *it;
		if ((position.YStart <= y) && (y <= position.YEnd) && (position.XStart <= x) && (x <= position.XEnd)) {
			return &position;
		}
	}
	return 0;
}

void TextView::gotoMark(TextMark mark) {
	if (mark.ParagraphNumber < 0) {
		return;
	}
	gotoParagraph(mark.ParagraphNumber);
	paint(false);
	while (mark > myLastParagraphCursor->position()) {
		scrollPageForward();
		paint(false);
	}
	repaintView();
}

void TextView::gotoParagraph(int num, bool last) {
	if (last) {
		if ((num > 0) && (num <= (int)myModel->paragraphs().size())) {
			if (myLastParagraphCursor == 0) {
				if (myFirstParagraphCursor == 0) {
					return;
				}
				myLastParagraphCursor = myFirstParagraphCursor;
				myFirstParagraphCursor = 0;
			}
			myLastParagraphCursor->moveTo(num - 1, 0, 0);
			myLastParagraphCursor->moveToParagraphEnd();
			if (myFirstParagraphCursor != 0) {
				delete myFirstParagraphCursor;
				myFirstParagraphCursor = 0;
			}
		}
	} else {
		if ((num >= 0) && (num < (int)myModel->paragraphs().size())) {
			if (myFirstParagraphCursor == 0) {
				if (myLastParagraphCursor == 0) {
					return;
				}
				myFirstParagraphCursor = myLastParagraphCursor;
				myLastParagraphCursor = 0;
			}
			myFirstParagraphCursor->moveTo(num, 0, 0);
		}
	}
}

void TextView::drawParagraph(ParagraphCursor &paragraph, bool doPaint) {
	myStyle.applyControls(paragraph.begin(), paragraph.wordCursor(), false);

	while (!paragraph.isEndOfParagraph()) {
		context().moveXTo(myStyle.style().leftIndent());

		const WordCursor lineStart = paragraph.wordCursor();
		const WordCursor lineEnd = myLineProcessor.process(lineStart, paragraph.end());
		context().moveY(myLineProcessor.height());
		int spaceCounter = myLineProcessor.spaceCounter();
		if (context().y() > myStyle.textAreaHeight()) {
			break;
		}
		myStyle.applyControls(lineStart, lineEnd, true);
		paragraph.setWordCursor(lineEnd);

		int fullCorrection = 0;
		switch (myStyle.style().alignment()) {
			case ALIGN_RIGHT:
				context().moveX(context().width() - myStyle.style().rightIndent() - myLineProcessor.width());
				break;
			case ALIGN_CENTER:
				context().moveX((context().width() - myStyle.style().rightIndent() - myLineProcessor.width()) / 2);
				break;
			case ALIGN_JUSTIFY:
				if (!paragraph.isEndOfParagraph() && (paragraph.wordCursor().element().kind() != TextElement::AFTER_PARAGRAPH_ELEMENT)) {
					fullCorrection = context().width() - myStyle.style().rightIndent() - myLineProcessor.width();
				}
				break;
			case ALIGN_LEFT:
			case ALIGN_UNDEFINED:
				break;
		}

		if (doPaint) {
			bool wordOccured = false;
			for (WordCursor pos = lineStart; !pos.sameElementAs(paragraph.wordCursor()); pos.nextWord()) {
				TextElement::Kind kind = pos.element().kind();
				int x = context().x();
				int y = context().y();
				int width = myStyle.elementWidth(pos);
				int height = myStyle.elementHeight(pos);
    
				switch (kind) {
					case TextElement::WORD_ELEMENT:
						wordOccured = true;
						y -= myStyle.style().verticalShift();
						context().setColor(myStyle.style().color());
						context().drawWord(x, y, (const Word&)pos.element(), pos.charNumber(), -1, false);
						break;
					case TextElement::IMAGE_ELEMENT:
						wordOccured = true;
						context().drawImage(x, y, ((const ImageElement&)pos.element()).image());
						break;
					case TextElement::CONTROL_ELEMENT:
						myStyle.applyControl((const ControlElement&)pos.element(), false);
						break;
					case TextElement::HSPACE_ELEMENT:
						if (wordOccured && (spaceCounter > 0)) {
							int correction = fullCorrection / spaceCounter;
							context().moveX(context().spaceWidth() + correction);
							fullCorrection -= correction;
							wordOccured = false;
							spaceCounter--;
						}
						break;
					case TextElement::TREE_ELEMENT:
						drawTreeNode(((const TreeElement&)pos.element()).treeElementKind());
						break;
					case TextElement::INDENT_ELEMENT:
					case TextElement::BEFORE_PARAGRAPH_ELEMENT:
					case TextElement::AFTER_PARAGRAPH_ELEMENT:
					case TextElement::EMPTY_LINE_ELEMENT:
						break;
				}
				context().moveX(width);
				if ((width > 0) && (height > 0)) {
					myTextElementMap.push_back(TextElementPosition(paragraph.paragraphNumber(), paragraph.wordNumber(pos), kind, x, x + width - 1, y - height + 1, y));
				}
			}
			if (!paragraph.isEndOfParagraph() && (paragraph.wordCursor().element().kind() == TextElement::WORD_ELEMENT)) {
				int len = paragraph.charNumber();
				if (len > 0) {
					const Word &word = (const Word&)paragraph.wordCursor().element();
					context().setColor(myStyle.style().color());
					context().drawWord(context().x(), context().y() - myStyle.style().verticalShift(), word, 0, len, word.charAt(len - 1) != '-');
				}
			}
		}
	}
	myStyle.reset();
}

void TextView::skip(ParagraphCursor &paragraph, int height) {
	while (!paragraph.isEndOfParagraph() && (height > 0)) {
		paragraph.setWordCursor(myLineProcessor.process(paragraph.wordCursor(), paragraph.end()));
		height -= myLineProcessor.height();
	}
	myStyle.reset();
}

int TextView::paragraphHeight(const ParagraphCursor &paragraph, bool beforeCurrentPosition) {
	WordCursor cursor = paragraph.begin();
	const WordCursor end = beforeCurrentPosition ? paragraph.wordCursor() : paragraph.end();
	
	int height = 0;

	while (!cursor.sameElementAs(end)) {
		cursor = myLineProcessor.process(cursor, end);
		height += myLineProcessor.height();
	}

	myStyle.reset();

	return height;
}

void TextView::search(const std::string &text, bool ignoreCase, bool wholeText, bool backward) {
	if (text.empty()) {
		return;
	}

	myModel->search(text, ignoreCase);
	if (myFirstParagraphCursor != 0) {
		TextMark position = myFirstParagraphCursor->position();
		gotoMark(wholeText ?
							(backward ? myModel->lastMark() : myModel->firstMark()) :
							(backward ? myModel->previousMark(position) : myModel->nextMark(position)));
	}
}

bool TextView::canFindNext() const {
	return
		(myLastParagraphCursor != 0) &&
		(myModel->nextMark(myLastParagraphCursor->position()).ParagraphNumber > -1);
}

void TextView::findNext() {
	if (myLastParagraphCursor != 0) {
		gotoMark(myModel->nextMark(myLastParagraphCursor->position()));
	}
}

bool TextView::canFindPrevious() const {
	return
		(myFirstParagraphCursor != 0) &&
		(myModel->previousMark(myFirstParagraphCursor->position()).ParagraphNumber > -1);
}

void TextView::findPrevious() {
	if (myFirstParagraphCursor != 0) {
		gotoMark(myModel->previousMark(myFirstParagraphCursor->position()));
	}
}

bool TextView::onStylusPress(int x, int y) {
	if (ShowPositionIndicatorOption.value() && (myModel != 0)) {
		int bottom = context().height();
		int top = bottom - PositionIndicatorHeightOption.value() + 1;
		int left = 0;
		int right = context().width() - 1;

		if ((x > left) && (x < right) && (y > top) && (y < bottom)) {
			int paragraphNumber = myModel->paragraphs().size() * (x - left - 1) / (right - left - 1);
			gotoParagraph(paragraphNumber, true);
			paint(false);
			if ((myLastParagraphCursor != 0) && !myLastParagraphCursor->isEndOfText()) {
				int paragraphLength = myLastParagraphCursor->paragraphLength();
				if (paragraphLength > 0) {
					int wordNum =
						(x - left - 1) * myModel->paragraphs().size() * paragraphLength / (right - left - 1) -
						myLastParagraphCursor->paragraphNumber() * paragraphLength;
					myLastParagraphCursor->moveTo(myLastParagraphCursor->paragraphNumber(), wordNum, 0);
					if (myFirstParagraphCursor != 0) {
						delete myFirstParagraphCursor;
						myFirstParagraphCursor = 0;
					}
					repaintView();
				}
			}
			return true;
		}
	}

	return false;
}
