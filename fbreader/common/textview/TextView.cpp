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

#include <algorithm>

#include <abstract/ZLUnicodeUtil.h>

#include "TextView.h"
#include "ParagraphCursor.h"
#include "TextStyle.h"
#include "Word.h"

#include "../model/TextModel.h"
#include "../model/Paragraph.h"

ZLColorOption TextView::TreeLinesColorOption("Colors", "TreeLines", ZLColor(127, 127, 127));
ZLBooleanOption TextView::AutoHyphenationOption("Options", "AutoHyphenation", true);

const std::string INDICATOR = "Indicator";

ZLBooleanOption TextView::ShowPositionIndicatorOption(INDICATOR, "Show", true);
ZLBooleanOption TextView::IsIndicatorSensitiveOption(INDICATOR, "TouchSensitive", true);
ZLColorOption TextView::PositionIndicatorColorOption(INDICATOR, "Color", ZLColor(127, 127, 127));
ZLIntegerOption TextView::PositionIndicatorHeightOption(INDICATOR, "Height", 16);
ZLIntegerOption TextView::PositionIndicatorOffsetOption(INDICATOR, "Offset", 4);

TextView::TextView(ZLPaintContext &context) : ZLView(context), myStyle(context), myLineProcessor(myStyle) {
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
	myTextSize.clear();
	myFullTextSize = 0;
}

void TextView::setModel(const TextModel *model, const std::string &name) {
	clear();

	myModel = model;

	if ((myModel != 0) && !myModel->paragraphs().empty()) {
		myFirstParagraphCursor = ParagraphCursor::createCursor(*myModel);
		myName = name;
		const std::vector<Paragraph*> &paragraphs = myModel->paragraphs();
		myTextSize.reserve(paragraphs.size() + 1);
		myTextSize.push_back(0);
		for (std::vector<Paragraph*>::const_iterator it = paragraphs.begin(); it != paragraphs.end(); it++) {
			myFullTextSize += (*it)->textLength();
			myTextSize.push_back(myFullTextSize);
		}
	}
}

void TextView::paint(bool doPaint) {
	myParagraphMap.clear();
	myTextElementMap.clear();

	context().setLeftMargin(TextStyle::LeftMarginOption.value());
	context().setRightMargin(TextStyle::RightMarginOption.value());
	context().setTopMargin(TextStyle::TopMarginOption.value());
	context().setBottomMargin(TextStyle::BottomMarginOption.value());
	context().clear(TextStyle::BackgroundColorOption.value());

	if (myFirstParagraphCursor == NULL) {
		if (myLastParagraphCursor == NULL) {
			return;
		}
		myFirstParagraphCursor = myLastParagraphCursor;
		myLastParagraphCursor = NULL;
		int height = paragraphHeight(*myFirstParagraphCursor, true);
		bool positionChanged = !myFirstParagraphCursor->isStartOfParagraph();
		myFirstParagraphCursor->moveToParagraphStart();
		const int textAreaHeight = myStyle.textAreaHeight();
		while (height < textAreaHeight) {
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
		skip(*myFirstParagraphCursor, height - textAreaHeight);
	}

	if (myLastParagraphCursor != NULL) {
		delete myLastParagraphCursor;
	}
	myLastParagraphCursor = myFirstParagraphCursor->createCopy();

	context().moveYTo(0);
	do {
		int start = context().y() + 1;
		drawParagraph(*myLastParagraphCursor, doPaint);
		if (doPaint) {
			myParagraphMap.push_back(ParagraphPosition(myLastParagraphCursor->paragraphNumber(), start, context().y()));
		}
	} while (myLastParagraphCursor->isEndOfParagraph() && myLastParagraphCursor->next() && !myLastParagraphCursor->isEndOfSection());

	if (doPaint && ShowPositionIndicatorOption.value()) {
		long bottom = context().height();
		long top = bottom - PositionIndicatorHeightOption.value() + 1;
		long left = 0;
		long right = context().width() - 1;
		long fillWidth;
		long paragraphLength = myLastParagraphCursor->paragraphLength();
		long sizeOfTextBeforeParagraph = myTextSize[myLastParagraphCursor->paragraphNumber()];
		if (paragraphLength == 0) {
			fillWidth = (long)(1.0 * (right - left - 1) * sizeOfTextBeforeParagraph / myFullTextSize);
		} else {
			long sizeOfParagraph = myTextSize[myLastParagraphCursor->paragraphNumber() + 1] - sizeOfTextBeforeParagraph;
			fillWidth = (long)((right - left - 1) * (sizeOfTextBeforeParagraph + 1.0 * sizeOfParagraph * myLastParagraphCursor->wordNumber() / paragraphLength) / myFullTextSize);
		}
		context().setColor(TextStyle::RegularTextColorOption.value());
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

void TextView::drawLine(const ParagraphCursor &paragraph, const WordCursor &from, const WordCursor &to) {
	int spaceCounter = myLineProcessor.spaceCounter();
	int fullCorrection = 0;
	const bool endOfParagraph = to.sameElementAs(paragraph.end());
	const int pn = paragraph.paragraphNumber();
	int wn = paragraph.wordNumber(from);
	bool wordOccured = false;

	switch (myStyle.style()->alignment()) {
		case ALIGN_RIGHT:
			context().moveX(context().width() - myStyle.style()->rightIndent() - myLineProcessor.width());
			break;
		case ALIGN_CENTER:
			context().moveX((context().width() - myStyle.style()->rightIndent() - myLineProcessor.width()) / 2);
			break;
		case ALIGN_JUSTIFY:
			if (!endOfParagraph && (to.element().kind() != TextElement::AFTER_PARAGRAPH_ELEMENT)) {
				fullCorrection = context().width() - myStyle.style()->rightIndent() - myLineProcessor.width();
			}
			break;
		case ALIGN_LEFT:
		case ALIGN_UNDEFINED:
			break;
	}

	for (WordCursor pos = from; !pos.sameElementAs(to); pos.nextWord(), wn++) {
		TextElement::Kind kind = pos.element().kind();
		int x = context().x();
		int y = context().y();
  
		switch (kind) {
			case TextElement::WORD_ELEMENT:
				wordOccured = true;
				y -= myStyle.style()->verticalShift();
				drawWord(x, y, (const Word&)pos.element(), pos.charNumber(), -1, false);
				break;
			case TextElement::IMAGE_ELEMENT:
				wordOccured = true;
				context().drawImage(x, y, ((const ImageElement&)pos.element()).image());
				break;
			case TextElement::CONTROL_ELEMENT:
				myStyle.applyControl((const ControlElement&)pos.element());
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

		int width = myStyle.elementWidth(pos);
		context().moveX(width);
		if (width > 0) {
			int height = myStyle.elementHeight(pos);
			if (height > 0) {
				myTextElementMap.push_back(TextElementPosition(pn, wn, kind, x, x + width - 1, y - height + 1, y));
			}
		}
	}
	if (!endOfParagraph && (to.element().kind() == TextElement::WORD_ELEMENT)) {
		int len = to.charNumber();
		if (len > 0) {
			const Word &word = (const Word&)to.element();
			context().setColor(myStyle.style()->color());
			ZLUnicodeUtil::Ucs2String ucs2string;
			ZLUnicodeUtil::utf8ToUcs2(ucs2string, word.Data, word.Size);
			drawWord(context().x(), context().y() - myStyle.style()->verticalShift(), word, 0, len, ucs2string[len - 1] != '-');
		}
	}
}

void TextView::drawParagraph(ParagraphCursor &paragraph, bool doPaint) {
	myStyle.reset();
	myStyle.applyControls(paragraph.begin(), paragraph.wordCursor());

	const int textAreaHeight = myStyle.textAreaHeight();

	while (!paragraph.isEndOfParagraph()) {
		const TextStylePtr storedStyle = myStyle.style();

		context().moveXTo(storedStyle->leftIndent());
		const WordCursor lineStart = paragraph.wordCursor();
		const WordCursor lineEnd = myLineProcessor.process(lineStart, paragraph.end());
		context().moveY(myLineProcessor.height());
		if (context().y() > textAreaHeight) {
			break;
		}
		paragraph.setWordCursor(lineEnd);

		if (doPaint) {
			myStyle.setStyle(storedStyle);
			drawLine(paragraph, lineStart, lineEnd);
		}
	}
}

void TextView::skip(ParagraphCursor &paragraph, int height) {
	myStyle.reset();
	myStyle.applyControls(paragraph.begin(), paragraph.wordCursor());
	while (!paragraph.isEndOfParagraph() && (height > 0)) {
		paragraph.setWordCursor(myLineProcessor.process(paragraph.wordCursor(), paragraph.end()));
		height -= myLineProcessor.height();
	}
}

int TextView::paragraphHeight(const ParagraphCursor &paragraph, bool beforeCurrentPosition) {
	myStyle.reset();
	WordCursor cursor = paragraph.begin();
	const WordCursor end = beforeCurrentPosition ? paragraph.wordCursor() : paragraph.end();
	
	int height = 0;

	while (!cursor.sameElementAs(end)) {
		cursor = myLineProcessor.process(cursor, end);
		height += myLineProcessor.height();
	}

	return height;
}

void TextView::search(const std::string &text, bool ignoreCase, bool wholeText, bool backward) {
	if (text.empty()) {
		return;
	}

	myModel->search(text, ignoreCase);
	if (myFirstParagraphCursor != 0) {
		myFirstParagraphCursor->rebuild();
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
	if (ShowPositionIndicatorOption.value() && IsIndicatorSensitiveOption.value() && (myTextSize.size() > 1)) {
		long bottom = context().height();
		long top = bottom - PositionIndicatorHeightOption.value() + 1;
		long left = 0;
		long right = context().width() - 1;

		if ((x > left) && (x < right) && (y > top) && (y < bottom)) {
			size_t textSize = (size_t)(1.0 * myFullTextSize * (x - left - 1) / (right - left - 1));
			std::vector<size_t>::const_iterator it = std::lower_bound(myTextSize.begin(), myTextSize.end(), textSize);
			long paragraphNumber = std::min((long)(it - myTextSize.begin()), (long)myTextSize.size() - 1) - 1;
			if (paragraphNumber == 0) {
				gotoParagraph(0, false);
				repaintView();
			} else {
				gotoParagraph(paragraphNumber, true);
				paint(false);
				if ((myLastParagraphCursor != 0) && (paragraphNumber == myLastParagraphCursor->paragraphNumber())) {
					if ((myLastParagraphCursor != 0) && !myLastParagraphCursor->isEndOfText()) {
						long paragraphLength = myLastParagraphCursor->paragraphLength();
						if (paragraphLength > 0) {
							long sizeOfTextBeforeParagraph = myTextSize[paragraphNumber];
							long sizeOfParagraph = myTextSize[paragraphNumber + 1] - sizeOfTextBeforeParagraph;
							long wordNum =
								(long)((1.0 * (x - left - 1) / (right - left - 1) - 1.0 * sizeOfTextBeforeParagraph / myFullTextSize)
								* myFullTextSize / sizeOfParagraph * paragraphLength);
							myLastParagraphCursor->moveTo(myLastParagraphCursor->paragraphNumber(), wordNum, 0);
							if (myFirstParagraphCursor != 0) {
								delete myFirstParagraphCursor;
								myFirstParagraphCursor = 0;
							}
							repaintView();
						}
					}
				} else {
					repaintView();
				}
			}
			return true;
		}
	}

	return false;
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
				context().setColor(TextStyle::SelectedTextColorOption.value());
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
	if (myFirstParagraphCursor != 0) {
		myFirstParagraphCursor->rebuild();
	}
	if (myLastParagraphCursor != 0) {
		myLastParagraphCursor->rebuild();
	}
}
