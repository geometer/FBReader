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

#include <abstract/ZLUnicodeUtil.h>

#include "TextView.h"
#include "ParagraphCursor.h"
#include "TextStyle.h"
#include "TextElement.h"

#include "../view/PaintContext.h"

TextView::ViewStyle::ViewStyle(PaintContext &context) : myContext(context) {
	myStyle = &TextStyleCollection::instance().baseStyle();
}

void TextView::ViewStyle::reset() {
	while (myStyle->isDecorated()) {
		DecoratedTextStyle *decorated = (DecoratedTextStyle*)myStyle;
		myStyle = &decorated->base();
		delete decorated;
	}
}

void TextView::ViewStyle::applyControl(const ControlElement &control, bool revert) {
	if (control.isStart() == revert) {
		if (myStyle->isDecorated()) {
			DecoratedTextStyle *decorated = (DecoratedTextStyle*)myStyle;
			myStyle = &decorated->base();
			delete decorated;
		}
	} else {
		const TextStyleDecoration *decoration = TextStyleCollection::instance().decoration(control.textKind());
		if (decoration != 0) {
			myStyle = decoration->createDecoratedStyle(*myStyle);
		}
	}
	myContext.setFont(style().fontFamily(), style().fontSize(), style().bold(), style().italic());
}

void TextView::ViewStyle::applyControls(const WordCursor &begin, const WordCursor &end, bool revert) {
	if (revert) {
		WordCursor cursor = end;
		while (!cursor.sameElementAs(begin)) {
			cursor.previousWord();
			if (cursor.element().kind() == TextElement::CONTROL_ELEMENT) {
				applyControl((ControlElement&)cursor.element(), true);
			}
		}
	} else {
		WordCursor cursor = begin;
		while (!cursor.sameElementAs(end)) {
			if (cursor.element().kind() == TextElement::CONTROL_ELEMENT) {
				applyControl((ControlElement&)cursor.element(), false);
			}
			cursor.nextWord();
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
			return style().firstLineIndentDelta();
		case TextElement::HSPACE_ELEMENT:
			return 0;
		case TextElement::BEFORE_PARAGRAPH_ELEMENT:
		case TextElement::AFTER_PARAGRAPH_ELEMENT:
		case TextElement::EMPTY_LINE_ELEMENT:
			return context().width() + abs(style().leftIndent()) + abs(style().rightIndent()) + abs(style().firstLineIndentDelta() + 1);
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
			return (int)(context().stringHeight() * style().lineSpace()) + style().verticalShift();
		case TextElement::TREE_ELEMENT:
			return context().stringHeight();
		case TextElement::IMAGE_ELEMENT:
		{
			int imageHeight = context().imageHeight(((const ImageElement&)element).image());
			int maxHeight = textAreaHeight();
			return (imageHeight < maxHeight) ? imageHeight : maxHeight;
		}
		case TextElement::BEFORE_PARAGRAPH_ELEMENT:
			return style().spaceBefore();
		case TextElement::AFTER_PARAGRAPH_ELEMENT:
			return style().spaceAfter();
		case TextElement::EMPTY_LINE_ELEMENT:
			return style().spaceBefore() + context().stringHeight() + style().spaceAfter();
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
	int startPos = ZLUnicodeUtil::length(word.data(), start);
	int endPos = (length == -1) ? word.size() : ZLUnicodeUtil::length(word.data(), start + length);
	if (!addHyphenationSign) {
		return context().stringWidth(word.data() + startPos, endPos - startPos);
	}
	std::string substr;
	substr.append(word.data() + startPos, endPos - startPos);
	substr += '-';
	return context().stringWidth(substr.data(), substr.length());
}

int TextView::ViewStyle::spaceWidth() const {
	return context().stringWidth(" ", 1);
}
