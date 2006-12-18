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

#include <algorithm>

#include <ZLUnicodeUtil.h>
#include <ZLPaintContext.h>

#include "TextView.h"
#include "ParagraphCursor.h"
#include "TextStyle.h"
#include "TextElement.h"

TextView::ViewStyle::ViewStyle(ZLPaintContext &context) : myContext(context) {
	setStyle(TextStyleCollection::instance().baseStylePtr());
	myWordHeight = -1;
}

void TextView::ViewStyle::reset() {
	setStyle(TextStyleCollection::instance().baseStylePtr());
}

void TextView::ViewStyle::setStyle(const TextStylePtr style) {
	if (myStyle != style) {
		myStyle = style;
		myWordHeight = -1;
	}
	myContext.setFont(myStyle->fontFamily(), myStyle->fontSize(), myStyle->bold(), myStyle->italic());
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

void TextView::ViewStyle::applyControl(const ForcedControlElement &control) {
	setStyle(new ForcedTextStyle(myStyle, control.entry()));
}

void TextView::ViewStyle::applyControls(const WordCursor &begin, const WordCursor &end) {
	for (WordCursor cursor = begin; !cursor.sameElementAs(end); cursor.nextWord()) {
		const TextElement &element = cursor.element();
		if (element.kind() == TextElement::CONTROL_ELEMENT) {
			applyControl((ControlElement&)element);
		} else if (element.kind() == TextElement::FORCED_CONTROL_ELEMENT) {
			applyControl((ForcedControlElement&)element);
		}
	}
}

int TextView::ViewStyle::elementWidth(const TextElement &element, unsigned int charNumber) const {
	switch (element.kind()) {
		case TextElement::WORD_ELEMENT:
			return wordWidth((const Word&)element, charNumber, -1, false);
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
		case TextElement::FORCED_CONTROL_ELEMENT:
		case TextElement::CONTROL_ELEMENT:
			return 0;
		case TextElement::FIXED_HSPACE_ELEMENT:
			return context().spaceWidth() * ((const FixedHSpaceElement&)element).length();
	}
	return 0;
}

int TextView::ViewStyle::elementHeight(const TextElement &element) const {
	switch (element.kind()) {
		case TextElement::WORD_ELEMENT:
			if (myWordHeight == -1) {
				myWordHeight = (int)(context().stringHeight() * style()->lineSpace()) + style()->verticalShift();
			}
			return myWordHeight;
		case TextElement::IMAGE_ELEMENT:
			return
				context().imageHeight(((const ImageElement&)element).image()) +
				std::max((int)(context().stringHeight() * (style()->lineSpace() - 1)), 3);
		case TextElement::BEFORE_PARAGRAPH_ELEMENT:
			return - style()->spaceAfter();
		case TextElement::AFTER_PARAGRAPH_ELEMENT:
			return - style()->spaceBefore();
		case TextElement::EMPTY_LINE_ELEMENT:
			return context().stringHeight();
		case TextElement::INDENT_ELEMENT:
		case TextElement::HSPACE_ELEMENT:
		case TextElement::FORCED_CONTROL_ELEMENT:
		case TextElement::CONTROL_ELEMENT:
		case TextElement::FIXED_HSPACE_ELEMENT:
			return 0;
	}
	return 0;
}

int TextView::ViewStyle::textAreaHeight() const {
	PositionIndicatorStyle &indicatorStyle = TextStyleCollection::instance().indicatorStyle();
	return indicatorStyle.ShowOption.value() ?
		context().height() - indicatorStyle.HeightOption.value() - indicatorStyle.OffsetOption.value() :
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
