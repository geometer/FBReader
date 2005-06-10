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

#include <abstract/ZLOptions.h>

#include "TextStyle.h"

TextStyleDecoration::TextStyleDecoration(const std::string &name, int fontSizeDelta, Boolean3 bold, Boolean3 italic, int verticalShift, Boolean3 allowHyphenations) :
	myName(name),
	myFontFamilyOption("Style", myName + ":fontFamily", std::string()),
	myFontSizeDeltaOption("Style", myName + ":fontSize", fontSizeDelta),
	myBoldOption("Style", myName + ":bold", bold),
	myItalicOption("Style", myName + ":italic", italic),
	myVerticalShiftOption("Style", myName + ":vShift", verticalShift),
	myAllowHyphenationsOption("Style", myName + ":allowHyphenations", allowHyphenations),
	myIsHyperlinkStyle(false) {
}

FullTextStyleDecoration::FullTextStyleDecoration(const std::string &name, int fontSizeDelta, Boolean3 bold, Boolean3 italic, int spaceBefore, int spaceAfter, int leftIndent, int rightIndent, int firstLineIndentDelta, int verticalShift, AlignmentType alignment, double lineSpace, Boolean3 allowHyphenations) : TextStyleDecoration(name, fontSizeDelta, bold, italic, verticalShift, allowHyphenations),
	mySpaceBeforeOption("Style", name + ":spaceBefore", spaceBefore),
	mySpaceAfterOption("Style", name + ":spaceAfter", spaceAfter),
	myLeftIndentOption("Style", name + ":leftIndent", leftIndent),
	myRightIndentOption("Style", name + ":rightIndent", rightIndent),
	myFirstLineIndentDeltaOption("Style", name + ":firstLineIndentDelta", firstLineIndentDelta),
	myAlignmentOption("Style", name + ":alignment", alignment),
	myLineSpaceOption("Style", name + ":lineSpace", lineSpace) {
}

TextStylePtr TextStyleDecoration::createDecoratedStyle(const TextStylePtr base) const {
	return new PartialDecoratedTextStyle(base, *this);
}

TextStylePtr FullTextStyleDecoration::createDecoratedStyle(const TextStylePtr base) const {
	return new FullDecoratedTextStyle(base, *this);
}

const std::string &PartialDecoratedTextStyle::fontFamily() const {
	const std::string &family = myDecoration.fontFamilyOption().value();
	return (!family.empty()) ? family : base()->fontFamily();
}

int PartialDecoratedTextStyle::fontSize() const {
	return base()->fontSize() + myDecoration.fontSizeDeltaOption().value();
}

bool PartialDecoratedTextStyle::bold() const {
	Boolean3 b = myDecoration.boldOption().value();
	return (b == B3_UNDEFINED) ? base()->bold() : (b == B3_TRUE);
}

bool PartialDecoratedTextStyle::italic() const {
	Boolean3 i = myDecoration.italicOption().value();
	return (i == B3_UNDEFINED) ? base()->italic() : (i == B3_TRUE);
}

bool PartialDecoratedTextStyle::allowHyphenations() const {
	Boolean3 a = myDecoration.allowHyphenationsOption().value();
	return (a == B3_UNDEFINED) ? base()->allowHyphenations() : (a == B3_TRUE);
	return true;
}

int FullDecoratedTextStyle::firstLineIndentDelta() const {
	return (alignment() == ALIGN_CENTER) ? 0 : base()->firstLineIndentDelta() + myDecoration.firstLineIndentDeltaOption().value();
}

const std::string &FullDecoratedTextStyle::fontFamily() const {
	const std::string &family = myDecoration.fontFamilyOption().value();
	return (!family.empty()) ? family : base()->fontFamily();
}

int FullDecoratedTextStyle::fontSize() const {
	return base()->fontSize() + myDecoration.fontSizeDeltaOption().value();
}

bool FullDecoratedTextStyle::bold() const {
	Boolean3 b = myDecoration.boldOption().value();
	return (b == B3_UNDEFINED) ? base()->bold() : (b == B3_TRUE);
}

bool FullDecoratedTextStyle::italic() const {
	Boolean3 i = myDecoration.italicOption().value();
	return (i == B3_UNDEFINED) ? base()->italic() : (i == B3_TRUE);
}

AlignmentType FullDecoratedTextStyle::alignment() const {
	AlignmentType a = (AlignmentType)myDecoration.alignmentOption().value();
	return (a == ALIGN_UNDEFINED) ? base()->alignment() : a;
}

bool FullDecoratedTextStyle::allowHyphenations() const {
	Boolean3 a = myDecoration.allowHyphenationsOption().value();
	return (a == B3_UNDEFINED) ? base()->allowHyphenations() : (a == B3_TRUE);
	return true;
}

ZLColor DecoratedTextStyle::color() const {
	return decoration().isHyperlinkStyle() ? HyperlinkTextColorOption.value() : base()->color();
}
