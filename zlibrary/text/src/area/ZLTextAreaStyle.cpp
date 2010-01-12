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
#include <ZLPaintContext.h>

#include <ZLTextStyle.h>
#include <ZLTextStyleCollection.h>

#include "ZLTextAreaStyle.h"
#include "ZLTextParagraphCursor.h"
#include "ZLTextElement.h"
#include "../style/ZLTextDecoratedStyle.h"

ZLTextArea::Style::Style(const ZLTextArea &area, shared_ptr<ZLTextStyle> style) : myArea(area) {
	myTextStyle = style;
	myWordHeight = -1;
	myArea.context().setFont(myTextStyle->fontFamily(), myTextStyle->fontSize(), myTextStyle->bold(), myTextStyle->italic());
	myBidiLevel = myArea.isRtl() ? 1 : 0;
}

void ZLTextArea::Style::setTextStyle(shared_ptr<ZLTextStyle> style, unsigned char bidiLevel) {
	if (myTextStyle != style) {
		myTextStyle = style;
		myWordHeight = -1;
	}
	myArea.context().setFont(myTextStyle->fontFamily(), myTextStyle->fontSize(), myTextStyle->bold(), myTextStyle->italic());
	myBidiLevel = bidiLevel;
}

void ZLTextArea::Style::applyControl(const ZLTextControlElement &control) {
	if (control.isStart()) {
		const ZLTextStyleDecoration *decoration = ZLTextStyleCollection::Instance().decoration(control.textKind());
		if (decoration != 0) {
			setTextStyle(decoration->createDecoratedStyle(myTextStyle), myBidiLevel);
		}
	} else {
		if (myTextStyle->isDecorated()) {
			setTextStyle(((ZLTextDecoratedStyle&)*myTextStyle).base(), myBidiLevel);
		}
	}
}

void ZLTextArea::Style::applyControl(const ZLTextStyleElement &control) {
	setTextStyle(new ZLTextForcedStyle(myTextStyle, control.entry()), myBidiLevel);
}

void ZLTextArea::Style::applySingleControl(const ZLTextElement &element) {
	switch (element.kind()) {
		case ZLTextElement::CONTROL_ELEMENT:
			applyControl((ZLTextControlElement&)element);
			break;
		case ZLTextElement::FORCED_CONTROL_ELEMENT:
			applyControl((ZLTextStyleElement&)element);
			break;
		case ZLTextElement::START_REVERSED_SEQUENCE_ELEMENT:
			increaseBidiLevel();
			break;
		case ZLTextElement::END_REVERSED_SEQUENCE_ELEMENT:
			decreaseBidiLevel();
			break;
		default:
			break;
	}
}

void ZLTextArea::Style::applyControls(const ZLTextWordCursor &begin, const ZLTextWordCursor &end) {
	for (ZLTextWordCursor cursor = begin; !cursor.equalElementIndex(end); cursor.nextWord()) {
		applySingleControl(cursor.element());
	}
}

int ZLTextArea::Style::elementWidth(const ZLTextElement &element, unsigned int charIndex, const ZLTextStyleEntry::Metrics &metrics) const {
	switch (element.kind()) {
		case ZLTextElement::WORD_ELEMENT:
			return wordWidth((const ZLTextWord&)element, charIndex, -1, false);
		case ZLTextElement::IMAGE_ELEMENT:
			return myArea.context().imageWidth(*((const ZLTextImageElement&)element).image(), myArea.width(), myArea.height(), ZLPaintContext::SCALE_REDUCE_SIZE);
		case ZLTextElement::INDENT_ELEMENT:
			return textStyle()->firstLineIndentDelta(metrics);
		case ZLTextElement::HSPACE_ELEMENT:
		case ZLTextElement::NB_HSPACE_ELEMENT:
			return 0;
		case ZLTextElement::BEFORE_PARAGRAPH_ELEMENT:
		case ZLTextElement::AFTER_PARAGRAPH_ELEMENT:
		case ZLTextElement::EMPTY_LINE_ELEMENT:
			return metrics.FullWidth + abs(textStyle()->lineStartIndent(metrics, false)) + abs(textStyle()->lineEndIndent(metrics, false)) + abs(textStyle()->firstLineIndentDelta(metrics)) + 1;
		case ZLTextElement::FORCED_CONTROL_ELEMENT:
		case ZLTextElement::CONTROL_ELEMENT:
		case ZLTextElement::START_REVERSED_SEQUENCE_ELEMENT:
		case ZLTextElement::END_REVERSED_SEQUENCE_ELEMENT:
			return 0;
		case ZLTextElement::FIXED_HSPACE_ELEMENT:
			return myArea.context().spaceWidth() * ((const ZLTextFixedHSpaceElement&)element).length();
	}
	return 0;
}

int ZLTextArea::Style::elementHeight(const ZLTextElement &element, const ZLTextStyleEntry::Metrics &metrics) const {
	switch (element.kind()) {
		case ZLTextElement::NB_HSPACE_ELEMENT:
		case ZLTextElement::WORD_ELEMENT:
			if (myWordHeight == -1) {
				myWordHeight = myArea.context().stringHeight() * textStyle()->lineSpacePercent() / 100 + textStyle()->verticalShift();
			}
			return myWordHeight;
		case ZLTextElement::IMAGE_ELEMENT:
			return
				myArea.context().imageHeight(*((const ZLTextImageElement&)element).image(), myArea.width(), myArea.height(), ZLPaintContext::SCALE_REDUCE_SIZE) +
				std::max(myArea.context().stringHeight() * (textStyle()->lineSpacePercent() - 100) / 100, 3);
		case ZLTextElement::BEFORE_PARAGRAPH_ELEMENT:
			return - textStyle()->spaceAfter(metrics);
		case ZLTextElement::AFTER_PARAGRAPH_ELEMENT:
			return - textStyle()->spaceBefore(metrics);
		case ZLTextElement::EMPTY_LINE_ELEMENT:
			return myArea.context().stringHeight();
		case ZLTextElement::INDENT_ELEMENT:
		case ZLTextElement::HSPACE_ELEMENT:
		case ZLTextElement::FORCED_CONTROL_ELEMENT:
		case ZLTextElement::CONTROL_ELEMENT:
		case ZLTextElement::FIXED_HSPACE_ELEMENT:
		case ZLTextElement::START_REVERSED_SEQUENCE_ELEMENT:
		case ZLTextElement::END_REVERSED_SEQUENCE_ELEMENT:
			return 0;
	}
	return 0;
}

int ZLTextArea::Style::elementDescent(const ZLTextElement &element) const {
	switch (element.kind()) {
		case ZLTextElement::WORD_ELEMENT:
			return myArea.context().descent();
		default:
			return 0;
	}
}

int ZLTextArea::Style::wordWidth(const ZLTextWord &word, int start, int length, bool addHyphenationSign) const {
	if ((start == 0) && (length == -1)) {
		return word.width(myArea.context());
	}
	int startPos = ZLUnicodeUtil::length(word.Data, start);
	int endPos = (length == -1) ? word.Size : ZLUnicodeUtil::length(word.Data, start + length);
	if (!addHyphenationSign) {
		return myArea.context().stringWidth(word.Data + startPos, endPos - startPos, word.BidiLevel % 2 == 1);
	}
	std::string substr;
	substr.append(word.Data + startPos, endPos - startPos);
	substr += '-';
	return myArea.context().stringWidth(substr.data(), substr.length(), word.BidiLevel % 2 == 1);
}

void ZLTextArea::Style::increaseBidiLevel() {
	++myBidiLevel;
}

void ZLTextArea::Style::decreaseBidiLevel() {
	unsigned char base = myArea.isRtl() ? 1 : 0;
	if (myBidiLevel > base) {
		--myBidiLevel;
	}
}
