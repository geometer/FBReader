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

#include <algorithm>

#include <ZLUnicodeUtil.h>
#include <ZLPaintContext.h>

#include "ZLTextView.h"
#include "ZLTextParagraphCursor.h"
#include "ZLTextStyle.h"
#include "ZLTextElement.h"

TextView::ViewStyle::ViewStyle(ZLPaintContext &context) : myContext(context) {
	setStyle(ZLTextStyleCollection::instance().baseStylePtr());
	myWordHeight = -1;
}

void TextView::ViewStyle::reset() {
	setStyle(ZLTextStyleCollection::instance().baseStylePtr());
}

void TextView::ViewStyle::setStyle(const ZLTextStylePtr style) {
	if (myStyle != style) {
		myStyle = style;
		myWordHeight = -1;
	}
	myContext.setFont(myStyle->fontFamily(), myStyle->fontSize(), myStyle->bold(), myStyle->italic());
}

void TextView::ViewStyle::applyControl(const ZLTextControlElement &control) {
	if (control.isStart()) {
		const ZLTextStyleDecoration *decoration = ZLTextStyleCollection::instance().decoration(control.textKind());
		if (decoration != 0) {
			setStyle(decoration->createDecoratedStyle(myStyle));
		}
	} else {
		if (myStyle->isDecorated()) {
			setStyle(((ZLTextDecoratedStyle&)*myStyle).base());
		}
	}
}

void TextView::ViewStyle::applyControl(const ZLTextForcedControlElement &control) {
	setStyle(new ZLTextForcedStyle(myStyle, control.entry()));
}

void TextView::ViewStyle::applyControls(const WordCursor &begin, const WordCursor &end) {
	for (WordCursor cursor = begin; !cursor.equalWordNumber(end); cursor.nextWord()) {
		const ZLTextElement &element = cursor.element();
		if (element.kind() == ZLTextElement::CONTROL_ELEMENT) {
			applyControl((ZLTextControlElement&)element);
		} else if (element.kind() == ZLTextElement::FORCED_CONTROL_ELEMENT) {
			applyControl((ZLTextForcedControlElement&)element);
		}
	}
}

int TextView::ViewStyle::elementWidth(const ZLTextElement &element, unsigned int charNumber) const {
	switch (element.kind()) {
		case ZLTextElement::WORD_ELEMENT:
			return wordWidth((const ZLTextWord&)element, charNumber, -1, false);
		case ZLTextElement::IMAGE_ELEMENT:
			return context().imageWidth(((const ZLTextImageElement&)element).image());
		case ZLTextElement::INDENT_ELEMENT:
			return style()->firstLineIndentDelta();
		case ZLTextElement::HSPACE_ELEMENT:
			return 0;
		case ZLTextElement::BEFORE_PARAGRAPH_ELEMENT:
		case ZLTextElement::AFTER_PARAGRAPH_ELEMENT:
		case ZLTextElement::EMPTY_LINE_ELEMENT:
			return context().width() + abs(style()->leftIndent()) + abs(style()->rightIndent()) + abs(style()->firstLineIndentDelta()) + 1;
		case ZLTextElement::FORCED_CONTROL_ELEMENT:
		case ZLTextElement::CONTROL_ELEMENT:
			return 0;
		case ZLTextElement::FIXED_HSPACE_ELEMENT:
			return context().spaceWidth() * ((const ZLTextFixedHSpaceElement&)element).length();
	}
	return 0;
}

int TextView::ViewStyle::elementHeight(const ZLTextElement &element) const {
	switch (element.kind()) {
		case ZLTextElement::WORD_ELEMENT:
			if (myWordHeight == -1) {
				myWordHeight = (int)(context().stringHeight() * style()->lineSpace()) + style()->verticalShift();
			}
			return myWordHeight;
		case ZLTextElement::IMAGE_ELEMENT:
			return
				context().imageHeight(((const ZLTextImageElement&)element).image()) +
				std::max((int)(context().stringHeight() * (style()->lineSpace() - 1)), 3);
		case ZLTextElement::BEFORE_PARAGRAPH_ELEMENT:
			return - style()->spaceAfter();
		case ZLTextElement::AFTER_PARAGRAPH_ELEMENT:
			return - style()->spaceBefore();
		case ZLTextElement::EMPTY_LINE_ELEMENT:
			return context().stringHeight();
		case ZLTextElement::INDENT_ELEMENT:
		case ZLTextElement::HSPACE_ELEMENT:
		case ZLTextElement::FORCED_CONTROL_ELEMENT:
		case ZLTextElement::CONTROL_ELEMENT:
		case ZLTextElement::FIXED_HSPACE_ELEMENT:
			return 0;
	}
	return 0;
}

int TextView::ViewStyle::elementDescent(const ZLTextElement &element) const {
	switch (element.kind()) {
		case ZLTextElement::WORD_ELEMENT:
			return context().descent();
		default:
			return 0;
	}
}

int TextView::ViewStyle::textAreaHeight() const {
	ZLTextPositionIndicatorStyle &indicatorStyle = ZLTextStyleCollection::instance().indicatorStyle();
	return indicatorStyle.ShowOption.value() ?
		context().height() - indicatorStyle.HeightOption.value() - indicatorStyle.OffsetOption.value() :
		context().height();
}

int TextView::ViewStyle::wordWidth(const ZLTextWord &word, int start, int length, bool addHyphenationSign) const {
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
