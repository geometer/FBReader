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

#include <ZLUnicodeUtil.h>

#include "ZLTextLineInfo.h"
#include "ZLTextAreaStyle.h"

void ZLTextArea::addRectangleToTextMap(Style &style, const ZLTextElementRectangle &rectangle) {
	const unsigned char index = style.bidiLevel() - (isRtl() ? 1 : 0);
	if (index > 0) {
		while (myTextElementsToRevert.size() < index) {
			myTextElementsToRevert.push_back(ZLTextElementMap());
		}
		myTextElementsToRevert[index - 1].push_back(rectangle);
	} else {
		myTextElementMap.push_back(rectangle);
	}
}

void ZLTextArea::flushRevertedElements(unsigned char bidiLevel) {
	const int index = (int)bidiLevel - (isRtl() ? 1 : 0);
	if ((index < 0) || (myTextElementsToRevert.size() <= (size_t)index)) {
		return;
	}
	ZLTextElementMap &from = myTextElementsToRevert[index];
	ZLTextElementMap &to = (index > 0) ?
		myTextElementsToRevert[index - 1] :
		myTextElementMap;
	if (!from.empty()) {
		const int sum = from[from.size() - 1].XEnd + from[0].XStart;
		for (ZLTextElementMap::reverse_iterator it = from.rbegin(); it != from.rend(); ++it) {
			int tmp = sum - it->XStart;
			it->XStart = sum - it->XEnd;
			it->XEnd = tmp;
			to.push_back(*it);
		}
		from.clear();
	}
}

void ZLTextArea::prepareTextLine(Style &style, const ZLTextLineInfo &info, int y) {
	y = std::min(y + info.Height, (int)height());

	style.setTextStyle(info.StartStyle, info.StartBidiLevel);
	int spaceCounter = info.SpaceCounter;
	int fullCorrection = 0;
	const bool endOfParagraph = info.End.isEndOfParagraph();
	bool wordOccured = false;

	int x = info.StartIndent;

	const int fontSize = style.textStyle()->fontSize();
	// TODO: change metrics at font change
	const ZLTextStyleEntry::Metrics metrics(fontSize, fontSize / 2, width(), height());

	switch (style.textStyle()->alignment()) {
		case ALIGN_RIGHT:
			if (!isRtl()) {
				x += metrics.FullWidth - style.textStyle()->lineEndIndent(metrics, false) - info.Width;
			}
			break;
		case ALIGN_LEFT:
			if (isRtl()) {
				x += metrics.FullWidth - style.textStyle()->lineEndIndent(metrics, true) - info.Width;
			}
			break;
		case ALIGN_LINESTART:
			break;
		case ALIGN_CENTER:
			x += (metrics.FullWidth - style.textStyle()->lineEndIndent(metrics, isRtl()) - info.Width) / 2;
			break;
		case ALIGN_JUSTIFY:
			if (!endOfParagraph && (info.End.element().kind() != ZLTextElement::AFTER_PARAGRAPH_ELEMENT)) {
				fullCorrection = metrics.FullWidth - style.textStyle()->lineEndIndent(metrics, isRtl()) - info.Width;
			}
			break;
		case ALIGN_UNDEFINED:
			break;
	}

	const ZLTextParagraphCursor &paragraph = info.RealStart.paragraphCursor();
	int paragraphIndex = paragraph.index();
	for (ZLTextWordCursor pos = info.RealStart; !pos.equalElementIndex(info.End); pos.nextWord()) {
		const ZLTextElement &element = paragraph[pos.elementIndex()];
		ZLTextElement::Kind kind = element.kind();
		int width = style.elementWidth(element, pos.charIndex(), metrics);
	
		style.applySingleControl(element);
		switch (kind) {
			case ZLTextElement::WORD_ELEMENT:
			case ZLTextElement::IMAGE_ELEMENT:
			{
				const int height = style.elementHeight(element, metrics);
				const int descent = style.elementDescent(element);
				const int length = (kind == ZLTextElement::WORD_ELEMENT) ? ((const ZLTextWord&)element).Length - pos.charIndex() : 0;
				addRectangleToTextMap(
					style,
					ZLTextElementRectangle(
						paragraphIndex, pos.elementIndex(), pos.charIndex(), length, false,
						style.textStyle(), kind,
						x, x + width - 1, y - height + 1, y + descent, style.bidiLevel()
					)
				);
				wordOccured = true;
				break;
			}
			case ZLTextElement::CONTROL_ELEMENT:
			case ZLTextElement::FORCED_CONTROL_ELEMENT:
				break;
			case ZLTextElement::HSPACE_ELEMENT:
			case ZLTextElement::NB_HSPACE_ELEMENT:
				if (wordOccured && (spaceCounter > 0)) {
					int correction = fullCorrection / spaceCounter;
					x += context().spaceWidth() + correction;
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
			case ZLTextElement::START_REVERSED_SEQUENCE_ELEMENT:
				//context().setColor(ZLColor(0, 255, 0));
				//context().drawLine(myHOffset + x, myVOffset + y, myHOffset + x, myVOffset + y - 20);
				break;
			case ZLTextElement::END_REVERSED_SEQUENCE_ELEMENT:
				flushRevertedElements(style.bidiLevel());
				//context().setColor(ZLColor(255, 0, 0));
				//context().drawLine(myHOffset + x, myVOffset + y, myHOffset + x, myVOffset + y - 20);
				break;
		}

		x += width;
	}
	if (!endOfParagraph && (info.End.element().kind() == ZLTextElement::WORD_ELEMENT)) {
		int start = 0;
		if (info.End.equalElementIndex(info.RealStart)) {
			start = info.RealStart.charIndex();
		}
		const int len = info.End.charIndex() - start;
		if (len > 0) {
			const ZLTextWord &word = (const ZLTextWord&)info.End.element();
			ZLUnicodeUtil::Ucs4String ucs4string;
			ZLUnicodeUtil::utf8ToUcs4(ucs4string, word.Data, word.Size);
			const bool addHyphenationSign = ucs4string[start + len - 1] != '-';
			const int width = style.wordWidth(word, start, len, addHyphenationSign);
			const int height = style.elementHeight(word, metrics);
			const int descent = style.elementDescent(word);
			addRectangleToTextMap(
				style,
				ZLTextElementRectangle(
					paragraphIndex, info.End.elementIndex(), start, len, addHyphenationSign,
					style.textStyle(), ZLTextElement::WORD_ELEMENT,
					x, x + width - 1, y - height + 1, y + descent, word.BidiLevel
				)
			);
		}
	}

	unsigned char minBidiLevel = isRtl() ? 1 : 0;
	for (unsigned char i = style.bidiLevel(); i > minBidiLevel; --i) {
		flushRevertedElements(i - 1);
	}
}
