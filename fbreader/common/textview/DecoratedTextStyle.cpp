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

TextStyleDecoration::TextStyleDecoration(const std::string &name, int fontSizeDelta, Boolean3 bold, Boolean3 italic, int spaceBefore, int spaceAfter, int leftIndent, int rightIndent, int firstLineIndentDelta, int verticalShift, AlignmentType alignment, double lineSpace, Boolean3 allowHyphenations) :
	myName(name),
	myFontFamilyOption("Style", myName + ":fontFamily", std::string()),
	myFontSizeDeltaOption("Style", myName + ":fontSize", fontSizeDelta),
	myBoldOption("Style", myName + ":bold", bold),
	myItalicOption("Style", myName + ":italic", italic),
	mySpaceBeforeOption("Style", myName + ":spaceBefore", spaceBefore),
	mySpaceAfterOption("Style", myName + ":spaceAfter", spaceAfter),
	myLeftIndentOption("Style", myName + ":leftIndent", leftIndent),
	myRightIndentOption("Style", myName + ":rightIndent", rightIndent),
	myFirstLineIndentDeltaOption("Style", myName + ":firstLineIndentDelta", firstLineIndentDelta),
	myVerticalShiftOption("Style", myName + ":vShift", verticalShift),
	myAlignmentOption("Style", myName + ":alignment", alignment),
	myLineSpaceOption("Style", myName + ":lineSpace", lineSpace),
	myAllowHyphenationsOption("Style", myName + ":allowHyphenations", allowHyphenations) {
}

int DecoratedTextStyle::firstLineIndentDelta() const {
	return (alignment() == ALIGN_CENTER) ? 0 : myBase.firstLineIndentDelta() + myDecoration.firstLineIndentDeltaOption().value();
}

const std::string DecoratedTextStyle::fontFamily() const {
	const std::string family = myDecoration.fontFamilyOption().value();
	return (!family.empty()) ? family : myBase.fontFamily();
}

int DecoratedTextStyle::fontSize() const {
	return myBase.fontSize() + myDecoration.fontSizeDeltaOption().value();
}

bool DecoratedTextStyle::bold() const {
	Boolean3 b = myDecoration.boldOption().value();
	return (b == B3_UNDEFINED) ? myBase.bold() : (b == B3_TRUE);
}

bool DecoratedTextStyle::italic() const {
	Boolean3 i = myDecoration.italicOption().value();
	return (i == B3_UNDEFINED) ? myBase.italic() : (i == B3_TRUE);
}

AlignmentType DecoratedTextStyle::alignment() const {
	AlignmentType a = (AlignmentType)myDecoration.alignmentOption().value();
	return (a == ALIGN_UNDEFINED) ? myBase.alignment() : a;
}

bool DecoratedTextStyle::allowHyphenations() const {
	Boolean3 a = myDecoration.allowHyphenationsOption().value();
	return (a == B3_UNDEFINED) ? myBase.allowHyphenations() : (a == B3_TRUE);
	return true;
}
