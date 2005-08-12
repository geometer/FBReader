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

TextView::TextView(ZLPaintContext &context) : ZLView(context), myModel(0), myStyle(context) {
}

TextView::~TextView() {
	clear();
}

void TextView::clear() {
	myStartCursor = 0;
	myEndCursor = 0;
	myParagraphMap.clear();
	myTextElementMap.clear();
	myTextSize.clear();
	myFullTextSize = 0;
	myLineInfos.clear();
}

void TextView::setModel(const TextModel *model, const std::string &name) {
	clear();

	myModel = model;

	if ((myModel != 0) && !myModel->paragraphs().empty()) {
		myStartCursor = ParagraphCursor::createCursor(*myModel);
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

void TextView::setStartCursor(int paragraphNumber, int wordNumber, int charNumber) {
	if (!myStartCursor.isNull()) {
		myStartCursor.moveToParagraph(paragraphNumber);
		myStartCursor.moveTo(wordNumber, charNumber);
		myEndCursor = 0;
		myLineInfos.clear();
	}
}

void TextView::paint(bool doPaint) {
	if (myLineInfos.empty()) {
		myParagraphMap.clear();
		myTextElementMap.clear();

		context().setLeftMargin(TextStyle::LeftMarginOption.value());
		context().setRightMargin(TextStyle::RightMarginOption.value());
		context().setTopMargin(TextStyle::TopMarginOption.value());
		context().setBottomMargin(TextStyle::BottomMarginOption.value());
		context().clear(TextStyle::BackgroundColorOption.value());

		if (myStartCursor.isNull()) {
			if (myEndCursor.isNull()) {
				return;
			}
			myStartCursor = myEndCursor;
			myEndCursor = 0;
			int height = paragraphHeight(myStartCursor, true);
			bool positionChanged = !myStartCursor.isStartOfParagraph();
			myStartCursor.moveToParagraphStart();
			const int textAreaHeight = myStyle.textAreaHeight();
			while (height < textAreaHeight) {
				if (positionChanged && myStartCursor.paragraphCursor().isEndOfSection()) {
					break;
				}
				if (!myStartCursor.previousParagraph()) {
					break;
				}
				if (!myStartCursor.paragraphCursor().isEndOfSection()) {
					positionChanged = true;
				}
				height += paragraphHeight(myStartCursor, false);
			}
			skip(myStartCursor, height - textAreaHeight);
		}

		myEndCursor = myStartCursor;

		int textAreaHeight = myStyle.textAreaHeight();
		do {
			LineInfo info(myEndCursor, myStyle.style());
			WordCursor paragraphEnd = myEndCursor;
			paragraphEnd.moveToParagraphEnd();
			WordCursor start = myEndCursor;
			start.moveToParagraphStart();

			myStyle.reset();
			myStyle.applyControls(start, info.Start);

			while (!info.End.isEndOfParagraph()) {
				info = processTextLine(info.End, paragraphEnd);
				textAreaHeight -= info.Height;
				if (textAreaHeight < 0) {
					break;
				}
				myEndCursor = info.End;
				myLineInfos.push_back(info);
			}
		} while (myEndCursor.isEndOfParagraph() && myEndCursor.nextParagraph() && !myEndCursor.paragraphCursor().isEndOfSection());
	}

	if (doPaint) {
		context().moveYTo(0);
		for (std::vector<LineInfo>::const_iterator it = myLineInfos.begin(); it != myLineInfos.end(); it++) {
			int start = context().y() + 1;
			drawTextLine(*it);
			myParagraphMap.push_back(
				ParagraphPosition(it->Start.paragraphCursor().paragraphNumber(), start, context().y())
			);
		}

		if (ShowPositionIndicatorOption.value()) {
			long bottom = context().height();
			long top = bottom - PositionIndicatorHeightOption.value() + 1;
			long left = 0;
			long right = context().width() - 1;
			long fillWidth;
			long paragraphLength = myEndCursor.paragraphCursor().paragraphLength();
			long paragraphNumber = myEndCursor.paragraphCursor().paragraphNumber();
			long sizeOfTextBeforeParagraph = myTextSize[paragraphNumber];
			if (paragraphLength == 0) {
				fillWidth = (long)(1.0 * (right - left - 1) * sizeOfTextBeforeParagraph / myFullTextSize);
			} else {
				long sizeOfParagraph = myTextSize[paragraphNumber + 1] - sizeOfTextBeforeParagraph;
				fillWidth = (long)((right - left - 1) * (sizeOfTextBeforeParagraph + 1.0 * sizeOfParagraph * myEndCursor.wordNumber() / paragraphLength) / myFullTextSize);
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
}

void TextView::scrollPageBackward() {
	if (!myStartCursor.isNull()) {
		if (!myStartCursor.paragraphCursor().isFirst() || !myStartCursor.isStartOfParagraph()) {
			myEndCursor = myStartCursor;
			myStartCursor = 0;
			myLineInfos.clear();
		}
	}
}

void TextView::scrollPageForward() {
	if (!myEndCursor.isNull()) {
		if (!myEndCursor.paragraphCursor().isLast() || !myEndCursor.isEndOfParagraph()) {
			myStartCursor = myEndCursor;
			myEndCursor = 0;
			myLineInfos.clear();
		}
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
	if (!myStartCursor.isNull() &&
			((myStartCursor.paragraphCursor().paragraphNumber() != mark.ParagraphNumber) ||
			 (myStartCursor.position() > mark))) {
		gotoParagraph(mark.ParagraphNumber);
		paint(false);
		while (mark > myEndCursor.position()) {
			scrollPageForward();
			paint(false);
		}
		repaintView();
	}
}

void TextView::gotoParagraph(int num, bool last) {
	if (last) {
		if ((num > 0) && (num <= (int)myModel->paragraphs().size())) {
			if (myEndCursor.isNull()) {
				if (myStartCursor.isNull()) {
					return;
				}
				myEndCursor = myStartCursor;
				myStartCursor = 0;
			}
			myEndCursor.moveToParagraph(num - 1);
			myEndCursor.moveToParagraphEnd();
			myStartCursor = 0;
		}
	} else {
		if ((num >= 0) && (num < (int)myModel->paragraphs().size())) {
			if (myStartCursor.isNull()) {
				if (myEndCursor.isNull()) {
					return;
				}
				myStartCursor = myEndCursor;
				myEndCursor = 0;
			}
			myStartCursor.moveToParagraph(num);
			myStartCursor.moveToParagraphStart();
		}
	}
	myLineInfos.clear();
}

void TextView::drawTextLine(const LineInfo &info) {
	myStyle.setStyle(info.StartStyle);
	context().moveXTo(info.StartStyle->leftIndent());
	context().moveY(info.Height);
	int spaceCounter = info.SpaceCounter;
	int fullCorrection = 0;
	const bool endOfParagraph = info.End.isEndOfParagraph();
	int pn = info.Start.paragraphCursor().paragraphNumber();
	int wn = info.Start.wordNumber();
	bool wordOccured = false;

	switch (myStyle.style()->alignment()) {
		case ALIGN_RIGHT:
			context().moveX(context().width() - myStyle.style()->rightIndent() - info.Width);
			break;
		case ALIGN_CENTER:
			context().moveX((context().width() - myStyle.style()->rightIndent() - info.Width) / 2);
			break;
		case ALIGN_JUSTIFY:
			if (!endOfParagraph && (info.End.element().kind() != TextElement::AFTER_PARAGRAPH_ELEMENT)) {
				fullCorrection = context().width() - myStyle.style()->rightIndent() - info.Width;
			}
			break;
		case ALIGN_LEFT:
		case ALIGN_UNDEFINED:
			break;
	}

	for (WordCursor pos = info.Start; !pos.sameElementAs(info.End); pos.nextWord(), wn++) {
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
				drawTreeNode(((const TreeElement&)pos.element()).treeElementKind(), info.Height);
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
	if (!endOfParagraph && (info.End.element().kind() == TextElement::WORD_ELEMENT)) {
		int len = info.End.charNumber();
		if (len > 0) {
			const Word &word = (const Word&)info.End.element();
			context().setColor(myStyle.style()->color());
			ZLUnicodeUtil::Ucs2String ucs2string;
			ZLUnicodeUtil::utf8ToUcs2(ucs2string, word.Data, word.Size);
			drawWord(context().x(), context().y() - myStyle.style()->verticalShift(), word, 0, len, ucs2string[len - 1] != '-');
		}
	}
}

void TextView::skip(WordCursor &cursor, int height) {
	WordCursor start = cursor;
	start.moveToParagraphStart();
	WordCursor paragraphEnd = cursor;
	paragraphEnd.moveToParagraphEnd();

	myStyle.reset();
	myStyle.applyControls(start, cursor);

	while (!cursor.isEndOfParagraph() && (height > 0)) {
		const LineInfo info = processTextLine(cursor, paragraphEnd);
		cursor = info.End;
		height -= info.Height;
	}
}

int TextView::paragraphHeight(const WordCursor &cursor, bool beforeCurrentPosition) {
	WordCursor word = cursor;
	word.moveToParagraphStart();
	WordCursor end = cursor;
	if (!beforeCurrentPosition) {
		end.moveToParagraphEnd();
	}
	
	myStyle.reset();

	int height = 0;

	while (!word.sameElementAs(end)) {
		LineInfo info = processTextLine(word, end);
		word = info.End;
		height += info.Height;
	}

	return height;
}

void TextView::search(const std::string &text, bool ignoreCase, bool wholeText, bool backward) {
	if (text.empty()) {
		return;
	}

	myModel->search(text, ignoreCase);
	if (!myStartCursor.isNull()) {
		clearCaches();
		TextMark position = myStartCursor.position();
		gotoMark(wholeText ?
							(backward ? myModel->lastMark() : myModel->firstMark()) :
							(backward ? myModel->previousMark(position) : myModel->nextMark(position)));
	}
}

bool TextView::canFindNext() const {
	return !myEndCursor.isNull() && (myModel->nextMark(myEndCursor.position()).ParagraphNumber > -1);
}

void TextView::findNext() {
	if (!myEndCursor.isNull()) {
		gotoMark(myModel->nextMark(myEndCursor.position()));
	}
}

bool TextView::canFindPrevious() const {
	return !myStartCursor.isNull() && (myModel->previousMark(myStartCursor.position()).ParagraphNumber > -1);
}

void TextView::findPrevious() {
	if (!myStartCursor.isNull()) {
		gotoMark(myModel->previousMark(myStartCursor.position()));
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
				if (!myEndCursor.isNull() && (paragraphNumber == myEndCursor.paragraphCursor().paragraphNumber())) {
					if (!myEndCursor.paragraphCursor().isLast() || !myEndCursor.isEndOfParagraph()) {
						long paragraphLength = myEndCursor.paragraphCursor().paragraphLength();
						if (paragraphLength > 0) {
							long sizeOfTextBeforeParagraph = myTextSize[paragraphNumber];
							long sizeOfParagraph = myTextSize[paragraphNumber + 1] - sizeOfTextBeforeParagraph;
							long wordNum =
								(long)((1.0 * (x - left - 1) / (right - left - 1) - 1.0 * sizeOfTextBeforeParagraph / myFullTextSize)
								* myFullTextSize / sizeOfParagraph * paragraphLength);
							myEndCursor.moveToParagraph(myEndCursor.paragraphCursor().paragraphNumber());
							myEndCursor.moveTo(wordNum, 0);
							myStartCursor = 0;
							myLineInfos.clear();
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
	myEndCursor = 0;
	myLineInfos.clear();
	myStartCursor.rebuild();
}
