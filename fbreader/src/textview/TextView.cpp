/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <ZLUnicodeUtil.h>

#include "TextView.h"
#include "ParagraphCursor.h"
#include "TextStyle.h"
#include "Word.h"
#include "SelectionModel.h"

#include "../model/TextModel.h"
#include "../model/Paragraph.h"

TextView::TextView(ZLApplication &application, ZLPaintContext &context) : ZLView(application, context), myPaintState(NOTHING_TO_PAINT), myOldWidth(-1), myOldHeight(-1), myStyle(context), mySelectionModel(*this), myTreeStateIsFrozen(false) {
}

TextView::~TextView() {
	clear();
}

void TextView::clear() {
	mySelectionModel.clear();

	myStartCursor = 0;
	myEndCursor = 0;
	myLineInfos.clear();
	myPaintState = NOTHING_TO_PAINT;

	myTextElementMap.clear();
	myTreeNodeMap.clear();
	myTextSize.clear();
	myTextBreaks.clear();

	ParagraphCursorCache::clear();
}

void TextView::setModel(shared_ptr<TextModel> model, const std::string &name) {
	clear();

	myModel = model;

	if (!myModel.isNull() && (myModel->paragraphsNumber() != 0)) {
		setStartCursor(ParagraphCursor::cursor(*myModel));

		myFileName = name;
		size_t size = myModel->paragraphsNumber();
		myTextSize.reserve(size + 1);
		myTextSize.push_back(0);
		for (size_t i= 0; i < size; ++i) {
			myTextSize.push_back(myTextSize.back() + (*myModel)[i]->textLength());
			if ((*myModel)[i]->kind() == Paragraph::END_OF_TEXT_PARAGRAPH) {
				myTextBreaks.push_back(i);
			}
		}
	}
}

bool operator <= (const TextElementArea &area, const SelectionModel::BoundElement &element) {
	return
		(area.ParagraphNumber < element.ParagraphNumber) ||
		((area.ParagraphNumber == element.ParagraphNumber) &&
		 (area.TextElementNumber <= element.TextElementNumber));
}

bool operator > (const TextElementArea &area, const SelectionModel::BoundElement &element) {
	return !(area <= element);
}

bool operator < (const WordCursor &cursor, const SelectionModel::BoundElement &element) {
	int pn = cursor.paragraphCursor().index();
	return
		(pn < element.ParagraphNumber) ||
		((pn == element.ParagraphNumber) &&
		 (((int)cursor.wordNumber() < element.TextElementNumber) ||
		  (((int)cursor.wordNumber() == element.TextElementNumber) &&
			 (cursor.charNumber() < element.CharNumber))));
}

bool operator >= (const WordCursor &cursor, const SelectionModel::BoundElement &element) {
	return !(cursor < element);
}

bool operator > (const WordCursor &cursor, const SelectionModel::BoundElement &element) {
	int pn = cursor.paragraphCursor().index();
	return
		(pn > element.ParagraphNumber) ||
		((pn == element.ParagraphNumber) &&
		 (((int)cursor.wordNumber() > element.TextElementNumber) ||
		  (((int)cursor.wordNumber() == element.TextElementNumber) &&
			 (cursor.charNumber() > element.CharNumber))));
}

bool operator <= (const WordCursor &cursor, const SelectionModel::BoundElement &element) {
	return !(cursor > element);
}

void TextView::paint() {
	preparePaintInfo();

	myTextElementMap.clear();
	myTreeNodeMap.clear();
	context().clear(TextStyleCollection::instance().baseStyle().BackgroundColorOption.value());

	if (empty()) {
		return;
	}

	std::vector<size_t> labels;
	labels.reserve(myLineInfos.size() + 1);
	labels.push_back(0);
	context().moveYTo(0);
	for (std::vector<LineInfoPtr>::const_iterator it = myLineInfos.begin(); it != myLineInfos.end(); ++it) {
		prepareTextLine(**it);
		labels.push_back(myTextElementMap.size());
	}
	mySelectionModel.update();
	context().moveYTo(0);
	int index = 0;
	for (std::vector<LineInfoPtr>::const_iterator it = myLineInfos.begin(); it != myLineInfos.end(); ++it) {
		drawTextLine(**it, labels[index], labels[index + 1]);
		++index;
	}

	PositionIndicatorStyle &indicatorStyle = TextStyleCollection::instance().indicatorStyle();
	if (indicatorStyle.ShowOption.value()) {
		positionIndicator().draw();
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
	repaintView();
}

int TextView::paragraphIndexByCoordinate(int y) const {
	int indexBefore = -1;
	for (TextElementIterator it = myTextElementMap.begin(); it != myTextElementMap.end(); ++it) {
		if (it->YEnd < y) {
			indexBefore = it->ParagraphNumber;
		} else if ((it->YStart <= y) || (it->ParagraphNumber == indexBefore)) {
			return it->ParagraphNumber;
		} else {
			return -1;
		}
	}
	return -1;
}

const TextElementArea *TextView::elementByCoordinates(int x, int y) const {
	TextElementIterator it =
		std::find_if(myTextElementMap.begin(), myTextElementMap.end(), TextElementArea::RangeChecker(x, y));
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
	if (myModel.isNull()) {
		return;
	}

	if (myModel->kind() == TextModel::TREE_MODEL) {
		if ((num >= 0) && (num < (int)myModel->paragraphsNumber())) {
			TreeParagraph *tp = (TreeParagraph*)(*myModel)[num];
			if (myTreeStateIsFrozen) {
				int corrected = num;
				TreeParagraph *parent = tp->parent();
				while ((corrected > 0) && (parent != 0) && !parent->isOpen()) {
					for (--corrected; ((corrected > 0) && parent != (*myModel)[corrected]); --corrected);
					parent = parent->parent();
				}
				if (last && (corrected != num)) {
					++corrected;
				}
				num = corrected;
			} else {
				tp->openTree();
				rebuildPaintInfo(true);
			}
		}
	}

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

void TextView::gotoPosition(int paragraphNumber, int wordNumber, int charNumber) {
	gotoParagraph(paragraphNumber, false);
	if (!myStartCursor.isNull() && 
			((int)myStartCursor.paragraphCursor().index() == paragraphNumber)) {
		moveStartCursor(paragraphNumber, wordNumber, charNumber);
	}
}

void TextView::prepareTextLine(const LineInfo &info) {
	myStyle.setStyle(info.StartStyle);
	const int y = std::min(context().y() + info.Height, myStyle.textAreaHeight());
	int spaceCounter = info.SpaceCounter;
	int fullCorrection = 0;
	const bool endOfParagraph = info.End.isEndOfParagraph();
	bool wordOccured = false;
	bool changeStyle = true;

	context().moveXTo(info.LeftIndent);

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

	const ParagraphCursor &paragraph = info.RealStart.paragraphCursor();
	int paragraphNumber = paragraph.index();
	for (WordCursor pos = info.RealStart; !pos.sameElementAs(info.End); pos.nextWord()) {
		const TextElement &element = paragraph[pos.wordNumber()];
		TextElement::Kind kind = element.kind();
		const int x = context().x();
		int width = myStyle.elementWidth(element, pos.charNumber());
	
		switch (kind) {
			case TextElement::WORD_ELEMENT:
			case TextElement::IMAGE_ELEMENT:
			{
				const int height = myStyle.elementHeight(element);
				const int descent = myStyle.elementDescent(element);
				const int length = (kind == TextElement::WORD_ELEMENT) ? ((const Word&)element).Length : 0;
				myTextElementMap.push_back(
					TextElementArea(
						paragraphNumber, pos.wordNumber(), pos.charNumber(), length, false,
						changeStyle, myStyle.style(), kind,
						x, x + width - 1, y - height + 1, y + descent
					)
				);
				changeStyle = false;
				wordOccured = true;
				break;
			}
			case TextElement::CONTROL_ELEMENT:
				myStyle.applyControl((const ControlElement&)element);
				changeStyle = true;
				break;
			case TextElement::FORCED_CONTROL_ELEMENT:
				myStyle.applyControl((const ForcedControlElement&)element);
				changeStyle = true;
				break;
			case TextElement::HSPACE_ELEMENT:
				if (wordOccured && (spaceCounter > 0)) {
					int correction = fullCorrection / spaceCounter;
					context().moveX(context().spaceWidth() + correction);
					fullCorrection -= correction;
					wordOccured = false;
					--spaceCounter;
				}
				break;
			case TextElement::INDENT_ELEMENT:
			case TextElement::BEFORE_PARAGRAPH_ELEMENT:
			case TextElement::AFTER_PARAGRAPH_ELEMENT:
			case TextElement::EMPTY_LINE_ELEMENT:
			case TextElement::FIXED_HSPACE_ELEMENT:
				break;
		}

		context().moveX(width);
	}
	if (!endOfParagraph && (info.End.element().kind() == TextElement::WORD_ELEMENT)) {
		int len = info.End.charNumber();
		if (len > 0) {
			const Word &word = (const Word&)info.End.element();
			ZLUnicodeUtil::Ucs2String ucs2string;
			ZLUnicodeUtil::utf8ToUcs2(ucs2string, word.Data, word.Size);
			const bool addHyphenationSign = ucs2string[len - 1] != '-';
			const int x = context().x(); 
			const int width = myStyle.wordWidth(word, 0, len, addHyphenationSign);
			const int height = myStyle.elementHeight(word);
			const int descent = myStyle.elementDescent(word);
			myTextElementMap.push_back(
				TextElementArea(
					paragraphNumber, info.End.wordNumber(), 0, len, addHyphenationSign,
					changeStyle, myStyle.style(), TextElement::WORD_ELEMENT,
					x, x + width - 1, y - height + 1, y + descent
				)
			);
		}
	}

	context().moveY(info.Height + info.Descent + info.VSpaceAfter);
}

static TextElementIterator findLast(TextElementIterator from, TextElementIterator to, const SelectionModel::BoundElement &bound) {
	if (*from > bound) {
		return from;
	}
	for (++from; (from != to) && (*from <= bound); ++from) {
	}
	return --from;
}

int TextView::areaLength(const ParagraphCursor &paragraph, const TextElementArea &area, int toCharNumber) {
	myStyle.setStyle(area.Style);
	const Word &word = (const Word&)paragraph[area.TextElementNumber];
	int length = toCharNumber - area.StartCharNumber;
	bool selectHyphenationSign = false;
	if (length >= area.Length) {
		selectHyphenationSign = area.AddHyphenationSign;
		length = area.Length;
	}
	if (length > 0) {
		return myStyle.wordWidth(word, area.StartCharNumber, length, selectHyphenationSign);
	}
	return 0;
}

void TextView::drawTextLine(const LineInfo &info, size_t from, size_t to) {
	const ParagraphCursor &paragraph = info.RealStart.paragraphCursor();

	const TextElementIterator fromIt = myTextElementMap.begin() + from;
	const TextElementIterator toIt = myTextElementMap.begin() + to;

	if (!mySelectionModel.isEmpty() && (from != to)) {
		std::pair<SelectionModel::BoundElement,SelectionModel::BoundElement> range = mySelectionModel.range();

		int left = context().width() - 1;
		if (info.Start > range.first) {
			left = 0;
		} else if (info.End >= range.first) {
			TextElementIterator jt = findLast(fromIt, toIt, range.first);
			left = jt->XStart;
			if (jt->Kind == TextElement::WORD_ELEMENT) {
				left += areaLength(paragraph, *jt, range.first.CharNumber);
			}
		}

		const int top = context().y() + 1;
		int bottom = context().y() + info.Height + info.Descent;
		int right = 0;
		if (info.End < range.second) {
			right = context().width() - 1;
			bottom += info.VSpaceAfter;
		} else if (info.Start <= range.second) {
			TextElementIterator jt = findLast(fromIt, toIt, range.second);
			if (jt->Kind == TextElement::WORD_ELEMENT) {
				right = jt->XStart + areaLength(paragraph, *jt, range.second.CharNumber) - 1;
			} else {
				right = jt->XEnd - 1;
			}
		}

		if (left < right) {
			context().setFillColor(TextStyleCollection::instance().baseStyle().SelectionBackgroundColorOption.value());
			context().fillRectangle(left, top, right, bottom);
		}
	}

	context().moveY(info.Height);
	int maxY = myStyle.textAreaHeight();
	if (context().y() > maxY) {
	  context().moveYTo(maxY);
	}
	context().moveXTo(0);
	if (!info.NodeInfo.isNull()) {
		drawTreeLines(*info.NodeInfo, info.Height, info.Descent + info.VSpaceAfter);
	}
	TextElementIterator it = fromIt;
	for (WordCursor pos = info.RealStart; !pos.sameElementAs(info.End); pos.nextWord()) {
		const TextElement &element = paragraph[pos.wordNumber()];
		TextElement::Kind kind = element.kind();
	
		if ((kind == TextElement::WORD_ELEMENT) || (kind == TextElement::IMAGE_ELEMENT)) {
			if (it->ChangeStyle) {
				myStyle.setStyle(it->Style);
			}
			const int x = it->XStart;
			const int y = it->YEnd - myStyle.elementDescent(element) - myStyle.style()->verticalShift();
			if (kind == TextElement::WORD_ELEMENT) {
				drawWord(x, y, (const Word&)element, pos.charNumber(), -1, false);
			} else {
				context().drawImage(x, y, ((const ImageElement&)element).image());
			}
			++it;
		}
	}
	if (it != toIt) {
		if (it->ChangeStyle) {
			myStyle.setStyle(it->Style);
		}
		int len = info.End.charNumber();
		const Word &word = (const Word&)info.End.element();
		context().setColor(myStyle.style()->color());
		const int x = it->XStart;
		const int y = it->YEnd - myStyle.elementDescent(word) - myStyle.style()->verticalShift();
		drawWord(x, y, word, 0, len, it->AddHyphenationSign);
	}
	context().moveY(info.Descent + info.VSpaceAfter);
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
		repaintView();
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
	mySelectionModel.deactivate();

  if (myModel.isNull()) {
	  return false;
	}

	PositionIndicatorStyle &indicatorStyle = TextStyleCollection::instance().indicatorStyle();
	if (indicatorStyle.ShowOption.value() && indicatorStyle.IsSensitiveOption.value()) {
		myTreeStateIsFrozen = true;
		bool indicatorAnswer = positionIndicator().onStylusPress(x, y);
		myTreeStateIsFrozen = false;
		if (indicatorAnswer) {
			return true;
		}
	}

	if (myModel->kind() == TextModel::TREE_MODEL) {
		TreeNodeMap::const_iterator it =
			std::find_if(myTreeNodeMap.begin(), myTreeNodeMap.end(), TreeNodeArea::RangeChecker(x, y));
		if (it != myTreeNodeMap.end()) {
			int paragraphNumber = it->ParagraphNumber;
			TreeParagraph *paragraph = (TreeParagraph*)(*myModel)[paragraphNumber];

			paragraph->open(!paragraph->isOpen());
			rebuildPaintInfo(true);
			preparePaintInfo();
			if (paragraph->isOpen()) {
				int nextParagraphNumber = paragraphNumber + paragraph->fullSize();
				int lastParagraphNumber = endCursor().paragraphCursor().index();
				if (endCursor().isEndOfParagraph()) {
					++lastParagraphNumber;
				}
				if (lastParagraphNumber < nextParagraphNumber) {
					gotoParagraph(nextParagraphNumber, true);
					preparePaintInfo();
				}
			}
			int firstParagraphNumber = startCursor().paragraphCursor().index();
			if (startCursor().isStartOfParagraph()) {
				--firstParagraphNumber;
			}
			if (firstParagraphNumber >= paragraphNumber) {
				gotoParagraph(paragraphNumber);
				preparePaintInfo();
			}
			repaintView();

			return true;
		}
	}

	return false;
}

void TextView::activateSelection(int x, int y) {
	mySelectionModel.activate(x, y);
	repaintView();
}

bool TextView::onStylusMovePressed(int x, int y) {
	if (mySelectionModel.extendTo(x, y)) {
		copySelectedTextToClipboard(ZLDialogManager::CLIPBOARD_SELECTION);
		repaintView();
	}
	return true;
}

bool TextView::hasSelectedText() const {
	return !mySelectionModel.isEmpty() && !mySelectionModel.getText().empty();
}

void TextView::copySelectedTextToClipboard(ZLDialogManager::ClipboardType type) const {
	if (ZLDialogManager::instance().isClipboardSupported(type)) {
		std::string text = mySelectionModel.getText();
		if (!text.empty()) {
			ZLDialogManager::instance().setClipboardText(text, type);
		}
	}
}

bool TextView::onStylusRelease(int, int) {
	mySelectionModel.deactivate();
	return true;
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
				context().setColor(TextStyleCollection::instance().baseStyle().SelectedTextColorOption.value());
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

void TextView::highlightParagraph(int paragraphNumber) {
	myModel->selectParagraph(paragraphNumber);
	rebuildPaintInfo(true);
}

TextView::LineInfo::~LineInfo() {
}
