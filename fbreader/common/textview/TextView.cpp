/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
ZLIntegerRangeOption TextView::PositionIndicatorHeightOption(INDICATOR, "Height", 1, 100, 16);
ZLIntegerRangeOption TextView::PositionIndicatorOffsetOption(INDICATOR, "Offset", 0, 100, 4);
	
TextView::TextView(ZLPaintContext &context) : ZLView(context), myModel(0), myPaintState(NOTHING_TO_PAINT), myOldWidth(-1), myOldHeight(-1), myStyle(context) {
}

TextView::~TextView() {
	clear();
}

void TextView::clear() {
	myStartCursor = 0;
	myEndCursor = 0;
	myLineInfos.clear();
	myPaintState = NOTHING_TO_PAINT;

	myParagraphMap.clear();
	myTextElementMap.clear();
	myTextSize.clear();
	myTextBreaks.clear();

	ParagraphCursorCache::clear();
}

void TextView::setModel(const TextModel *model, const std::string &name) {
	clear();

	myModel = model;

	if ((myModel != 0) && (myModel->paragraphsNumber() != 0)) {
		setStartCursor(ParagraphCursor::createCursor(*myModel));

		myFileName = name;
		size_t size = myModel->paragraphsNumber();
		myTextSize.reserve(size + 1);
		myTextSize.push_back(0);
		for (size_t i= 0; i < size; i++) {
			myTextSize.push_back(myTextSize.back() + (*myModel)[i]->textLength());
			if ((*myModel)[i]->kind() == Paragraph::END_OF_TEXT_PARAGRAPH) {
				myTextBreaks.push_back(i);
			}
		}
	}
}

void TextView::paint() {
	preparePaintInfo();

	myParagraphMap.clear();
	myTextElementMap.clear();
	context().clear(TextStyle::BackgroundColorOption.value());

	if (empty()) {
		return;
	}
	context().moveYTo(0);
	for (std::vector<LineInfo>::const_iterator it = myLineInfos.begin(); it != myLineInfos.end(); ++it) {
		drawTextLine(*it);
	}

	if (ShowPositionIndicatorOption.value()) {
		long bottom = context().height();
		long top = bottom - PositionIndicatorHeightOption.value() + 1;
		long left = 0;
		long right = context().width() - 1;

		std::vector<size_t>::const_iterator i = nextBreakIterator();
		size_t startIndex = (i != myTextBreaks.begin()) ? *(i - 1) : 0;
		size_t endIndex = (i != myTextBreaks.end()) ? *i : myModel->paragraphsNumber();

		size_t paragraphNumber = endCursor().paragraphCursor().index();
		size_t sizeOfTextBeforeParagraph = myTextSize[paragraphNumber] - myTextSize[startIndex];
		size_t fullTextSize = myTextSize[endIndex] - myTextSize[startIndex];
		size_t paragraphLength = endCursor().paragraphCursor().paragraphLength();
		size_t fillWidth;

		if (paragraphLength == 0) {
			fillWidth = (size_t)(1.0 * (right - left - 1) * sizeOfTextBeforeParagraph / fullTextSize);
		} else {
			size_t sizeOfParagraph = myTextSize[paragraphNumber + 1] - myTextSize[paragraphNumber];
			fillWidth = (size_t)(
				(sizeOfTextBeforeParagraph + 1.0 * sizeOfParagraph * endCursor().wordNumber() / paragraphLength) *
				(right - left - 1) / fullTextSize
			);
		}
		context().setColor(TextStyle::RegularTextColorOption.value());
		context().setFillColor(PositionIndicatorColorOption.value());
		context().fillRectangle(left + 1, top + 1, left + fillWidth + 1, bottom - 1);
		context().drawLine(left, top, right, top);
		context().drawLine(left, bottom, right, bottom);
		context().drawLine(left, bottom, left, top);
		context().drawLine(right, bottom, right, top);
	}

	ParagraphCursorCache::cleanup();
}

void TextView::scrollPage(bool forward, ScrollingMode mode, unsigned int value) {
	preparePaintInfo();
	if (myPaintState == READY) {
		myPaintState = forward ? TO_SCROLL_FORWARD : TO_SCROLL_BACKWARD;
		myScrollingMode = mode;
		myOverlappingValue = value;
	}
}

std::vector<size_t>::const_iterator TextView::nextBreakIterator() const {
	return std::lower_bound(myTextBreaks.begin(), myTextBreaks.end(), endCursor().paragraphCursor().index());
}

void TextView::scrollToHome() {
	if (!startCursor().isNull() &&
			startCursor().isStartOfParagraph() &&
			startCursor().paragraphCursor().index() == 0) {
		return;
	}

	gotoParagraph(0, false);
	repaintView();
}

void TextView::scrollToStartOfText() {
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
	repaintView();
}

void TextView::scrollToEndOfText() {
	if (endCursor().isNull() || (myModel == 0)) {
		return;
	}

	if (endCursor().isEndOfParagraph() &&
			endCursor().paragraphCursor().isLast()) {
		return;
	}

	std::vector<size_t>::const_iterator i = nextBreakIterator();
	gotoParagraph((i != myTextBreaks.end()) ? *i : myModel->paragraphsNumber(), true);
	repaintView();
}

const TextView::ParagraphPosition *TextView::paragraphByCoordinate(int y) const {
	std::vector<ParagraphPosition>::const_iterator it =
		std::find_if(myParagraphMap.begin(), myParagraphMap.end(), ParagraphPosition::RangeChecker(y));
	return (it != myParagraphMap.end()) ? &*it : 0;
}

const TextView::TextElementPosition *TextView::elementByCoordinates(int x, int y) const {
	std::vector<TextElementPosition>::const_iterator it =
		std::find_if(myTextElementMap.begin(), myTextElementMap.end(), TextElementPosition::RangeChecker(x, y));
	return (it != myTextElementMap.end()) ? &*it : 0;
}

void TextView::gotoMark(TextMark mark) {
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
		repaintView();
	}
}

void TextView::gotoParagraph(int num, bool last) {
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

void TextView::drawTextLine(const LineInfo &info) {
	myStyle.setStyle(info.StartStyle);
	context().moveXTo(info.StartStyle->leftIndent());
	int y = context().y();
	myParagraphMap.push_back(
		ParagraphPosition(info.Start.paragraphCursor().index(), y + 1, y + info.Height)
	);
	context().moveY(info.Height);
	int spaceCounter = info.SpaceCounter;
	int fullCorrection = 0;
	const bool endOfParagraph = info.End.isEndOfParagraph();
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

	const ParagraphCursor &paragraph = info.Start.paragraphCursor();
	int paragraphNumber = paragraph.index();
	for (WordCursor pos = info.Start; !pos.sameElementAs(info.End); pos.nextWord()) {
		const TextElement &element = paragraph[pos.wordNumber()];
		TextElement::Kind kind = element.kind();
		int x = context().x();
		int y = context().y();
  
		switch (kind) {
			case TextElement::WORD_ELEMENT:
				wordOccured = true;
				y -= myStyle.style()->verticalShift();
				drawWord(x, y, (const Word&)element, pos.charNumber(), -1, false);
				break;
			case TextElement::IMAGE_ELEMENT:
				wordOccured = true;
				context().drawImage(x, y, ((const ImageElement&)element).image());
				break;
			case TextElement::CONTROL_ELEMENT:
				myStyle.applyControl((const ControlElement&)element);
				break;
			case TextElement::FORCED_CONTROL_ELEMENT:
				myStyle.applyControl((const ForcedControlElement&)element);
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
				drawTreeNode(((const TreeElement&)element).treeElementKind(), info.Height);
				break;
			case TextElement::INDENT_ELEMENT:
			case TextElement::BEFORE_PARAGRAPH_ELEMENT:
			case TextElement::AFTER_PARAGRAPH_ELEMENT:
			case TextElement::EMPTY_LINE_ELEMENT:
				break;
		}

		int width = myStyle.elementWidth(element, pos.charNumber());
		context().moveX(width);
		if (width > 0) {
			int height = myStyle.elementHeight(element);
			if (height > 0) {
				myTextElementMap.push_back(TextElementPosition(paragraphNumber, pos.wordNumber(), kind, x, x + width - 1, y - height + 1, y));
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

bool TextView::hasMultiSectionModel() const {
	return !myTextBreaks.empty();
}

void TextView::search(const std::string &text, bool ignoreCase, bool wholeText, bool backward, bool thisSectionOnly) {
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
		TextMark position = startCursor().position();
		gotoMark(wholeText ?
							(backward ? myModel->lastMark() : myModel->firstMark()) :
							(backward ? myModel->previousMark(position) : myModel->nextMark(position)));
	}
}

bool TextView::canFindNext() const {
	return !endCursor().isNull() && (myModel->nextMark(endCursor().position()).ParagraphNumber > -1);
}

void TextView::findNext() {
	if (!endCursor().isNull()) {
		gotoMark(myModel->nextMark(endCursor().position()));
	}
}

bool TextView::canFindPrevious() const {
	return !startCursor().isNull() && (myModel->previousMark(startCursor().position()).ParagraphNumber > -1);
}

void TextView::findPrevious() {
	if (!startCursor().isNull()) {
		gotoMark(myModel->previousMark(startCursor().position()));
	}
}

bool TextView::onStylusPress(int x, int y) {
	if (ShowPositionIndicatorOption.value() && IsIndicatorSensitiveOption.value() && (myTextSize.size() > 1)) {
		long bottom = context().height();
		long top = bottom - PositionIndicatorHeightOption.value() + 1;
		long left = 0;
		long right = context().width() - 1;

		if ((x > left) && (x < right) && (y > top) && (y < bottom)) {
			std::vector<size_t>::const_iterator i = nextBreakIterator();
			size_t startIndex = (i != myTextBreaks.begin()) ? *(i - 1) : 0;
			size_t endIndex = (i != myTextBreaks.end()) ? *i : myModel->paragraphsNumber();

			size_t fullTextSize = myTextSize[endIndex] - myTextSize[startIndex];
			size_t textSize = (size_t)(1.0 * fullTextSize * (x - left - 1) / (right - left - 1)) + myTextSize[startIndex];
			std::vector<size_t>::const_iterator it = std::lower_bound(myTextSize.begin(), myTextSize.end(), textSize);
			size_t paragraphNumber = std::min((size_t)(it - myTextSize.begin()), endIndex) - 1;
			if (paragraphNumber == startIndex) {
				gotoParagraph(startIndex, false);
				repaintView();
			} else {
				gotoParagraph(paragraphNumber, true);
				preparePaintInfo();
				if (!endCursor().isNull() && (paragraphNumber == endCursor().paragraphCursor().index())) {
					if (!endCursor().paragraphCursor().isLast() || !endCursor().isEndOfParagraph()) {
						size_t paragraphLength = endCursor().paragraphCursor().paragraphLength();
						if (paragraphLength > 0) {
							size_t sizeOfTextBeforeParagraph = myTextSize[paragraphNumber] - myTextSize[startIndex];
							size_t sizeOfParagraph = myTextSize[paragraphNumber + 1] - myTextSize[paragraphNumber];
							size_t wordNum =
								(size_t)((1.0 * (x - left - 1) / (right - left - 1) * fullTextSize
													- 1.0 * sizeOfTextBeforeParagraph)
												 / sizeOfParagraph * paragraphLength);
							moveEndCursor(endCursor().paragraphCursor().index(), wordNum, 0);
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
	rebuildPaintInfo(true);
}

void TextView::selectParagraph(int paragraphNumber) {
	myModel->selectParagraph(paragraphNumber);
	rebuildPaintInfo(true);
}
