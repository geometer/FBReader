/*
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

#include <ZLUnicodeUtil.h>

#include "ZLTextView.h"
#include "ZLTextLineInfo.h"

void TextView::paint() {
	preparePaintInfo();

	myTextElementMap.clear();
	myTreeNodeMap.clear();
	context().clear(ZLTextStyleCollection::instance().baseStyle().BackgroundColorOption.value());

	if (empty()) {
		return;
	}

	std::vector<size_t> labels;
	labels.reserve(myLineInfos.size() + 1);
	labels.push_back(0);
	context().moveYTo(0);
	for (std::vector<ZLTextLineInfoPtr>::const_iterator it = myLineInfos.begin(); it != myLineInfos.end(); ++it) {
		prepareTextLine(**it);
		labels.push_back(myTextElementMap.size());
	}
	mySelectionModel.update();
	context().moveYTo(0);
	int index = 0;
	for (std::vector<ZLTextLineInfoPtr>::const_iterator it = myLineInfos.begin(); it != myLineInfos.end(); ++it) {
		drawTextLine(**it, labels[index], labels[index + 1]);
		++index;
	}

	ZLTextPositionIndicatorStyle &indicatorStyle = ZLTextStyleCollection::instance().indicatorStyle();
	if (indicatorStyle.ShowOption.value()) {
		positionIndicator().draw();
	}

	ZLTextParagraphCursorCache::cleanup();
}

static bool operator <= (const TextElementArea &area, const SelectionModel::BoundElement &element) {
	return
		(area.ParagraphNumber < element.ParagraphNumber) ||
		((area.ParagraphNumber == element.ParagraphNumber) &&
		 (area.TextElementNumber <= element.TextElementNumber));
}

static bool operator > (const TextElementArea &area, const SelectionModel::BoundElement &element) {
	return !(area <= element);
}

static bool operator < (const ZLTextWordCursor &cursor, const SelectionModel::BoundElement &element) {
	int pn = cursor.paragraphCursor().index();
	return
		(pn < element.ParagraphNumber) ||
		((pn == element.ParagraphNumber) &&
		 (((int)cursor.wordNumber() < element.TextElementNumber) ||
		  (((int)cursor.wordNumber() == element.TextElementNumber) &&
			 (cursor.charNumber() < element.CharNumber))));
}

static bool operator >= (const ZLTextWordCursor &cursor, const SelectionModel::BoundElement &element) {
	return !(cursor < element);
}

static bool operator > (const ZLTextWordCursor &cursor, const SelectionModel::BoundElement &element) {
	int pn = cursor.paragraphCursor().index();
	return
		(pn > element.ParagraphNumber) ||
		((pn == element.ParagraphNumber) &&
		 (((int)cursor.wordNumber() > element.TextElementNumber) ||
		  (((int)cursor.wordNumber() == element.TextElementNumber) &&
			 (cursor.charNumber() > element.CharNumber))));
}

static bool operator <= (const ZLTextWordCursor &cursor, const SelectionModel::BoundElement &element) {
	return !(cursor > element);
}

static TextElementIterator findLast(TextElementIterator from, TextElementIterator to, const SelectionModel::BoundElement &bound) {
	if (*from > bound) {
		return from;
	}
	for (++from; (from != to) && (*from <= bound); ++from) {
	}
	return --from;
}

int TextView::areaLength(const ZLTextParagraphCursor &paragraph, const TextElementArea &area, int toCharNumber) {
	myStyle.setStyle(area.Style);
	const ZLTextWord &word = (const ZLTextWord&)paragraph[area.TextElementNumber];
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

void TextView::drawTextLine(const ZLTextLineInfo &info, size_t from, size_t to) {
	const ZLTextParagraphCursor &paragraph = info.RealStart.paragraphCursor();

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
			if (jt->Kind == ZLTextElement::WORD_ELEMENT) {
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
			if (jt->Kind == ZLTextElement::WORD_ELEMENT) {
				right = jt->XStart + areaLength(paragraph, *jt, range.second.CharNumber) - 1;
			} else {
				right = jt->XEnd - 1;
			}
		}

		if (left < right) {
			context().setFillColor(ZLTextStyleCollection::instance().baseStyle().SelectionBackgroundColorOption.value());
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
	for (ZLTextWordCursor pos = info.RealStart; !pos.equalWordNumber(info.End); pos.nextWord()) {
		const ZLTextElement &element = paragraph[pos.wordNumber()];
		ZLTextElement::Kind kind = element.kind();
	
		if ((kind == ZLTextElement::WORD_ELEMENT) || (kind == ZLTextElement::IMAGE_ELEMENT)) {
			if (it->ChangeStyle) {
				myStyle.setStyle(it->Style);
			}
			const int x = it->XStart;
			const int y = it->YEnd - myStyle.elementDescent(element) - myStyle.style()->verticalShift();
			if (kind == ZLTextElement::WORD_ELEMENT) {
				drawWord(x, y, (const ZLTextWord&)element, pos.charNumber(), -1, false);
			} else {
				context().drawImage(x, y, ((const ZLTextImageElement&)element).image());
			}
			++it;
		}
	}
	if (it != toIt) {
		if (it->ChangeStyle) {
			myStyle.setStyle(it->Style);
		}
		int len = info.End.charNumber();
		const ZLTextWord &word = (const ZLTextWord&)info.End.element();
		context().setColor(myStyle.style()->color());
		const int x = it->XStart;
		const int y = it->YEnd - myStyle.elementDescent(word) - myStyle.style()->verticalShift();
		drawWord(x, y, word, 0, len, it->AddHyphenationSign);
	}
	context().moveY(info.Descent + info.VSpaceAfter);
}

void TextView::prepareTextLine(const ZLTextLineInfo &info) {
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
			if (!endOfParagraph && (info.End.element().kind() != ZLTextElement::AFTER_PARAGRAPH_ELEMENT)) {
				fullCorrection = context().width() - myStyle.style()->rightIndent() - info.Width;
			}
			break;
		case ALIGN_LEFT:
		case ALIGN_UNDEFINED:
			break;
	}

	const ZLTextParagraphCursor &paragraph = info.RealStart.paragraphCursor();
	int paragraphNumber = paragraph.index();
	for (ZLTextWordCursor pos = info.RealStart; !pos.equalWordNumber(info.End); pos.nextWord()) {
		const ZLTextElement &element = paragraph[pos.wordNumber()];
		ZLTextElement::Kind kind = element.kind();
		const int x = context().x();
		int width = myStyle.elementWidth(element, pos.charNumber());
	
		switch (kind) {
			case ZLTextElement::WORD_ELEMENT:
			case ZLTextElement::IMAGE_ELEMENT:
			{
				const int height = myStyle.elementHeight(element);
				const int descent = myStyle.elementDescent(element);
				const int length = (kind == ZLTextElement::WORD_ELEMENT) ? ((const ZLTextWord&)element).Length : 0;
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
			case ZLTextElement::CONTROL_ELEMENT:
				myStyle.applyControl((const ZLTextControlElement&)element);
				changeStyle = true;
				break;
			case ZLTextElement::FORCED_CONTROL_ELEMENT:
				myStyle.applyControl((const ZLTextForcedControlElement&)element);
				changeStyle = true;
				break;
			case ZLTextElement::HSPACE_ELEMENT:
				if (wordOccured && (spaceCounter > 0)) {
					int correction = fullCorrection / spaceCounter;
					context().moveX(context().spaceWidth() + correction);
					fullCorrection -= correction;
					wordOccured = false;
					--spaceCounter;
				}
				break;
			case ZLTextElement::INDENT_ELEMENT:
			case ZLTextElement::BEFORE_PARAGRAPH_ELEMENT:
			case ZLTextElement::AFTER_PARAGRAPH_ELEMENT:
			case ZLTextElement::EMPTY_LINE_ELEMENT:
			case ZLTextElement::FIXED_HSPACE_ELEMENT:
				break;
		}

		context().moveX(width);
	}
	if (!endOfParagraph && (info.End.element().kind() == ZLTextElement::WORD_ELEMENT)) {
		int len = info.End.charNumber();
		if (len > 0) {
			const ZLTextWord &word = (const ZLTextWord&)info.End.element();
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
					changeStyle, myStyle.style(), ZLTextElement::WORD_ELEMENT,
					x, x + width - 1, y - height + 1, y + descent
				)
			);
		}
	}

	context().moveY(info.Height + info.Descent + info.VSpaceAfter);
}
