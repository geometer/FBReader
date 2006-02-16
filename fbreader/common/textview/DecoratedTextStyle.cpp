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

#include <abstract/ZLOptions.h>

#include "TextStyle.h"
#include "../model/Paragraph.h"

TextStyleDecoration::TextStyleDecoration(const std::string &name, int fontSizeDelta, Boolean3 bold, Boolean3 italic, int verticalShift, Boolean3 allowHyphenations) :
	myName(name),
	myFontFamilyOption("Style", myName + ":fontFamily", std::string()),
	myFontSizeDeltaOption("Style", myName + ":fontSize", -16, 16, fontSizeDelta),
	myBoldOption("Style", myName + ":bold", bold),
	myItalicOption("Style", myName + ":italic", italic),
	myVerticalShiftOption("Style", myName + ":vShift", verticalShift),
	myAllowHyphenationsOption("Style", myName + ":allowHyphenations", allowHyphenations),
	myIsHyperlinkStyle(false) {
}

FullTextStyleDecoration::FullTextStyleDecoration(const std::string &name, int fontSizeDelta, Boolean3 bold, Boolean3 italic, int spaceBefore, int spaceAfter, int leftIndent, int rightIndent, int firstLineIndentDelta, int verticalShift, AlignmentType alignment, double lineSpace, Boolean3 allowHyphenations) : TextStyleDecoration(name, fontSizeDelta, bold, italic, verticalShift, allowHyphenations),
	mySpaceBeforeOption("Style", name + ":spaceBefore", -10, 100, spaceBefore),
	mySpaceAfterOption("Style", name + ":spaceAfter", -10, 100, spaceAfter),
	myLeftIndentOption("Style", name + ":leftIndent", -300, 300, leftIndent),
	myRightIndentOption("Style", name + ":rightIndent", -300, 300, rightIndent),
	myFirstLineIndentDeltaOption("Style", name + ":firstLineIndentDelta", -300, 300, firstLineIndentDelta),
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

ZLColor PartialDecoratedTextStyle::color() const {
	return myDecoration.isHyperlinkStyle() ? HyperlinkTextColorOption.value() : base()->color();
}

ZLColor FullDecoratedTextStyle::color() const {
	return myDecoration.isHyperlinkStyle() ? HyperlinkTextColorOption.value() : base()->color();
}

int ForcedTextStyle::leftIndent() const {
	return myEntry.leftIndentSupported() ? myEntry.leftIndent() : base()->leftIndent();
}

int ForcedTextStyle::rightIndent() const {
	return myEntry.rightIndentSupported() ? myEntry.rightIndent() : base()->rightIndent();
}

AlignmentType ForcedTextStyle::alignment() const {
	return myEntry.alignmentTypeSupported() ? myEntry.alignmentType() : base()->alignment();
}
