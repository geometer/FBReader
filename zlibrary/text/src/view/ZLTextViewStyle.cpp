/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

ZLTextView::ViewStyle::ViewStyle(shared_ptr<ZLPaintContext> context) : myContext(context) {
	setTextStyle(ZLTextStyleCollection::instance().baseStylePtr());
	myWordHeight = -1;
}

void ZLTextView::ViewStyle::setPaintContext(shared_ptr<ZLPaintContext> context) {
	myContext = context;
}

void ZLTextView::ViewStyle::reset() {
	setTextStyle(ZLTextStyleCollection::instance().baseStylePtr());
}

void ZLTextView::ViewStyle::setTextStyle(const ZLTextStylePtr style) {
	if (myTextStyle != style) {
		myTextStyle = style;
		myWordHeight = -1;
	}
	if (!myContext.isNull()) {
		myContext->setFont(myTextStyle->fontFamily(), myTextStyle->fontSize(), myTextStyle->bold(), myTextStyle->italic());
	}
}

void ZLTextView::ViewStyle::applyControl(const ZLTextControlElement &control) {
	if (control.isStart()) {
		const ZLTextStyleDecoration *decoration = ZLTextStyleCollection::instance().decoration(control.textKind());
		if (decoration != 0) {
			setTextStyle(decoration->createDecoratedStyle(myTextStyle));
		}
	} else {
		if (myTextStyle->isDecorated()) {
			setTextStyle(((ZLTextDecoratedStyle&)*myTextStyle).base());
		}
	}
}

void ZLTextView::ViewStyle::applyControl(const ZLTextForcedControlElement &control) {
	setTextStyle(new ZLTextForcedStyle(myTextStyle, control.entry()));
}

void ZLTextView::ViewStyle::applyControls(const ZLTextWordCursor &begin, const ZLTextWordCursor &end) {
	for (ZLTextWordCursor cursor = begin; !cursor.equalWordNumber(end); cursor.nextWord()) {
		const ZLTextElement &element = cursor.element();
		if (element.kind() == ZLTextElement::CONTROL_ELEMENT) {
			applyControl((ZLTextControlElement&)element);
		} else if (element.kind() == ZLTextElement::FORCED_CONTROL_ELEMENT) {
			applyControl((ZLTextForcedControlElement&)element);
		}
	}
}

int ZLTextView::ViewStyle::elementWidth(const ZLTextElement &element, unsigned int charNumber) const {
	switch (element.kind()) {
		case ZLTextElement::WORD_ELEMENT:
			return wordWidth((const ZLTextWord&)element, charNumber, -1, false);
		case ZLTextElement::IMAGE_ELEMENT:
			return context().imageWidth(((const ZLTextImageElement&)element).image());
		case ZLTextElement::INDENT_ELEMENT:
			return textStyle()->firstLineIndentDelta();
		case ZLTextElement::HSPACE_ELEMENT:
		case ZLTextElement::NB_HSPACE_ELEMENT:
			return 0;
		case ZLTextElement::BEFORE_PARAGRAPH_ELEMENT:
		case ZLTextElement::AFTER_PARAGRAPH_ELEMENT:
		case ZLTextElement::EMPTY_LINE_ELEMENT:
			return context().width() + abs(textStyle()->leftIndent()) + abs(textStyle()->rightIndent()) + abs(textStyle()->firstLineIndentDelta()) + 1;
		case ZLTextElement::FORCED_CONTROL_ELEMENT:
		case ZLTextElement::CONTROL_ELEMENT:
			return 0;
		case ZLTextElement::FIXED_HSPACE_ELEMENT:
			return context().spaceWidth() * ((const ZLTextFixedHSpaceElement&)element).length();
	}
	return 0;
}

int ZLTextView::ViewStyle::elementHeight(const ZLTextElement &element) const {
	switch (element.kind()) {
		case ZLTextElement::WORD_ELEMENT:
			if (myWordHeight == -1) {
				myWordHeight = context().stringHeight() * textStyle()->lineSpacePercent() / 100 + textStyle()->verticalShift();
			}
			return myWordHeight;
		case ZLTextElement::IMAGE_ELEMENT:
			return
				context().imageHeight(((const ZLTextImageElement&)element).image()) +
				std::max(context().stringHeight() * (textStyle()->lineSpacePercent() - 100) / 100, 3);
		case ZLTextElement::BEFORE_PARAGRAPH_ELEMENT:
			return - textStyle()->spaceAfter();
		case ZLTextElement::AFTER_PARAGRAPH_ELEMENT:
			return - textStyle()->spaceBefore();
		case ZLTextElement::EMPTY_LINE_ELEMENT:
			return context().stringHeight();
		case ZLTextElement::INDENT_ELEMENT:
		case ZLTextElement::HSPACE_ELEMENT:
		case ZLTextElement::NB_HSPACE_ELEMENT:
		case ZLTextElement::FORCED_CONTROL_ELEMENT:
		case ZLTextElement::CONTROL_ELEMENT:
		case ZLTextElement::FIXED_HSPACE_ELEMENT:
			return 0;
	}
	return 0;
}

int ZLTextView::ViewStyle::elementDescent(const ZLTextElement &element) const {
	switch (element.kind()) {
		case ZLTextElement::WORD_ELEMENT:
			return context().descent();
		default:
			return 0;
	}
}

int ZLTextView::ViewStyle::wordWidth(const ZLTextWord &word, int start, int length, bool addHyphenationSign) const {
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
