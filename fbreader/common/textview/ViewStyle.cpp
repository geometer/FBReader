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
#include <abstract/ZLPaintContext.h>

#include "TextView.h"
#include "ParagraphCursor.h"
#include "TextStyle.h"
#include "TextElement.h"

TextView::ViewStyle::ViewStyle(ZLPaintContext &context) : myContext(context) {
	myStyle = TextStyleCollection::instance().baseStyle();
	myWordHeight = -1;
}

void TextView::ViewStyle::reset() {
	setStyle(TextStyleCollection::instance().baseStyle());
}

void TextView::ViewStyle::setStyle(const TextStylePtr style) {
	if (myStyle != style) {
		myStyle = style;
		myContext.setFont(myStyle->fontFamily(), myStyle->fontSize(), myStyle->bold(), myStyle->italic());
		myWordHeight = -1;
	}
}

void TextView::ViewStyle::applyControl(const ControlElement &control) {
	if (control.isStart()) {
		const TextStyleDecoration *decoration = TextStyleCollection::instance().decoration(control.textKind());
		if (decoration != 0) {
			setStyle(decoration->createDecoratedStyle(myStyle));
		}
	} else {
		if (myStyle->isDecorated()) {
			setStyle(((DecoratedTextStyle&)*myStyle).base());
		}
	}
}

void TextView::ViewStyle::applyControls(const WordCursor &begin, const WordCursor &end) {
	for (WordCursor cursor = begin; !cursor.sameElementAs(end); cursor.nextWord()) {
		if (cursor.element().kind() == TextElement::CONTROL_ELEMENT) {
			applyControl((ControlElement&)cursor.element());
		}
	}
}

int TextView::ViewStyle::elementWidth(const WordCursor &cursor) const {
	const TextElement &element = cursor.element();
	switch (element.kind()) {
		case TextElement::WORD_ELEMENT:
			return wordWidth((const Word&)element, cursor.charNumber(), -1, false);
		case TextElement::IMAGE_ELEMENT:
			return context().imageWidth(((const ImageElement&)element).image());
		case TextElement::INDENT_ELEMENT:
			return style()->firstLineIndentDelta();
		case TextElement::HSPACE_ELEMENT:
			return 0;
		case TextElement::BEFORE_PARAGRAPH_ELEMENT:
		case TextElement::AFTER_PARAGRAPH_ELEMENT:
		case TextElement::EMPTY_LINE_ELEMENT:
			return context().width() + abs(style()->leftIndent()) + abs(style()->rightIndent()) + abs(style()->firstLineIndentDelta()) + 1;
		case TextElement::TREE_ELEMENT:
			return context().stringHeight() * 4 / 3;
		case TextElement::CONTROL_ELEMENT:
			return 0;
	}
	return 0;
}

int TextView::ViewStyle::elementHeight(const WordCursor &cursor) const {
	const TextElement &element = cursor.element();
	switch (element.kind()) {
		case TextElement::WORD_ELEMENT:
			if (myWordHeight == -1) {
				myWordHeight = (int)(context().stringHeight() * style()->lineSpace()) + style()->verticalShift();
			}
			return myWordHeight;
		case TextElement::TREE_ELEMENT:
			return context().stringHeight();
		case TextElement::IMAGE_ELEMENT:
			return std::min(context().imageHeight(((const ImageElement&)element).image()), textAreaHeight());
		case TextElement::BEFORE_PARAGRAPH_ELEMENT:
			return style()->spaceBefore();
		case TextElement::AFTER_PARAGRAPH_ELEMENT:
			return style()->spaceAfter();
		case TextElement::EMPTY_LINE_ELEMENT:
			return style()->spaceBefore() + context().stringHeight() + style()->spaceAfter();
		case TextElement::INDENT_ELEMENT:
		case TextElement::HSPACE_ELEMENT:
		case TextElement::CONTROL_ELEMENT:
			return 0;
	}
	return 0;
}

int TextView::ViewStyle::textAreaHeight() const {
	return TextView::ShowPositionIndicatorOption.value() ?
		context().height() - PositionIndicatorHeightOption.value() - PositionIndicatorOffsetOption.value() :
		context().height();
}

int TextView::ViewStyle::wordWidth(const Word &word, int start, int length, bool addHyphenationSign) const {
	if ((start == 0) && (length == -1)) {
		return word.width(context());
	}
	int startPos = ZLUnicodeUtil::length(word.Data, start);
	int endPos = (length == -1) ? word.Size : ZLUnicodeUtil::length(word.Data, start + length);
	if (!addHyphenationSign) {
		return context().stringWidth(word.Data + startPos, endPos - startPos);
	}
	std::string substr;
	substr.append(word.Data + startPos, endPos - startPos);
	substr += '-';
	return context().stringWidth(substr.data(), substr.length());
}
