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

#include <ZLOptions.h>

#include <ZLTextParagraph.h>

#include "ZLTextStyle.h"

static const std::string STYLE = "Style";

ZLTextStyleDecoration::ZLTextStyleDecoration(const std::string &name, int fontSizeDelta, ZLBoolean3 bold, ZLBoolean3 italic, int verticalShift, ZLBoolean3 allowHyphenations) :
	FontFamilyOption(ZLCategoryKey::LOOK_AND_FEEL, STYLE, name + ":fontFamily", std::string()),
	FontSizeDeltaOption(ZLCategoryKey::LOOK_AND_FEEL, STYLE, name + ":fontSize", -16, 16, fontSizeDelta),
	BoldOption(ZLCategoryKey::LOOK_AND_FEEL, STYLE, name + ":bold", bold),
	ItalicOption(ZLCategoryKey::LOOK_AND_FEEL, STYLE, name + ":italic", italic),
	VerticalShiftOption(ZLCategoryKey::LOOK_AND_FEEL, STYLE, name + ":vShift", verticalShift),
	AllowHyphenationsOption(ZLCategoryKey::LOOK_AND_FEEL, STYLE, name + ":allowHyphenations", allowHyphenations),
	myName(name),
	myHyperlinkStyle(NONE) {
}

ZLTextFullStyleDecoration::ZLTextFullStyleDecoration(const std::string &name, int fontSizeDelta, ZLBoolean3 bold, ZLBoolean3 italic, int spaceBefore, int spaceAfter, int leftIndent, int rightIndent, int firstLineIndentDelta, int verticalShift, ZLTextAlignmentType alignment, double lineSpace, ZLBoolean3 allowHyphenations) : ZLTextStyleDecoration(name, fontSizeDelta, bold, italic, verticalShift, allowHyphenations),
	SpaceBeforeOption(ZLCategoryKey::LOOK_AND_FEEL, STYLE, name + ":spaceBefore", -10, 100, spaceBefore),
	SpaceAfterOption(ZLCategoryKey::LOOK_AND_FEEL, STYLE, name + ":spaceAfter", -10, 100, spaceAfter),
	LeftIndentOption(ZLCategoryKey::LOOK_AND_FEEL, STYLE, name + ":leftIndent", -300, 300, leftIndent),
	RightIndentOption(ZLCategoryKey::LOOK_AND_FEEL, STYLE, name + ":rightIndent", -300, 300, rightIndent),
	FirstLineIndentDeltaOption(ZLCategoryKey::LOOK_AND_FEEL, STYLE, name + ":firstLineIndentDelta", -300, 300, firstLineIndentDelta),
	AlignmentOption(ZLCategoryKey::LOOK_AND_FEEL, STYLE, name + ":alignment", alignment),
	LineSpaceOption(ZLCategoryKey::LOOK_AND_FEEL, STYLE, name + ":lineSpace", lineSpace),
	LineSpacePercentOption(ZLCategoryKey::LOOK_AND_FEEL, STYLE, name + ":lineSpacePercent", (lineSpace == 0.0) ? -1 : (int)(lineSpace * 100)) {
}

ZLTextStylePtr ZLTextStyleDecoration::createDecoratedStyle(const ZLTextStylePtr base) const {
	return new ZLTextPartialDecoratedStyle(base, *this);
}

ZLTextStylePtr ZLTextFullStyleDecoration::createDecoratedStyle(const ZLTextStylePtr base) const {
	return new ZLTextFullDecoratedStyle(base, *this);
}

const std::string &ZLTextPartialDecoratedStyle::fontFamily() const {
	const std::string &family = myDecoration.FontFamilyOption.value();
	return (!family.empty()) ? family : base()->fontFamily();
}

int ZLTextPartialDecoratedStyle::fontSize() const {
	return base()->fontSize() + myDecoration.FontSizeDeltaOption.value();
}

bool ZLTextPartialDecoratedStyle::bold() const {
	ZLBoolean3 b = myDecoration.BoldOption.value();
	return (b == B3_UNDEFINED) ? base()->bold() : (b == B3_TRUE);
}

bool ZLTextPartialDecoratedStyle::italic() const {
	ZLBoolean3 i = myDecoration.ItalicOption.value();
	return (i == B3_UNDEFINED) ? base()->italic() : (i == B3_TRUE);
}

bool ZLTextPartialDecoratedStyle::allowHyphenations() const {
	ZLBoolean3 a = myDecoration.AllowHyphenationsOption.value();
	return (a == B3_UNDEFINED) ? base()->allowHyphenations() : (a == B3_TRUE);
	return true;
}

int ZLTextFullDecoratedStyle::firstLineIndentDelta() const {
	return (alignment() == ALIGN_CENTER) ? 0 : base()->firstLineIndentDelta() + myDecoration.FirstLineIndentDeltaOption.value();
}

const std::string &ZLTextFullDecoratedStyle::fontFamily() const {
	const std::string &family = myDecoration.FontFamilyOption.value();
	return (!family.empty()) ? family : base()->fontFamily();
}

int ZLTextFullDecoratedStyle::fontSize() const {
	return base()->fontSize() + myDecoration.FontSizeDeltaOption.value();
}

bool ZLTextFullDecoratedStyle::bold() const {
	ZLBoolean3 b = myDecoration.BoldOption.value();
	return (b == B3_UNDEFINED) ? base()->bold() : (b == B3_TRUE);
}

bool ZLTextFullDecoratedStyle::italic() const {
	ZLBoolean3 i = myDecoration.ItalicOption.value();
	return (i == B3_UNDEFINED) ? base()->italic() : (i == B3_TRUE);
}

ZLTextAlignmentType ZLTextFullDecoratedStyle::alignment() const {
	ZLTextAlignmentType a = (ZLTextAlignmentType)myDecoration.AlignmentOption.value();
	return (a == ALIGN_UNDEFINED) ? base()->alignment() : a;
}

bool ZLTextFullDecoratedStyle::allowHyphenations() const {
	ZLBoolean3 a = myDecoration.AllowHyphenationsOption.value();
	return (a == B3_UNDEFINED) ? base()->allowHyphenations() : (a == B3_TRUE);
	return true;
}

ZLColor ZLTextPartialDecoratedStyle::color() const {
	ZLTextStyleDecoration::HyperlinkStyle hyperlinkStyle = myDecoration.hyperlinkStyle();
	if (hyperlinkStyle == ZLTextStyleDecoration::NONE) {
		return base()->color();
	}
	ZLTextBaseStyle &baseStyle = ZLTextStyleCollection::instance().baseStyle();
	if (hyperlinkStyle == ZLTextStyleDecoration::INTERNAL) {
		return baseStyle.InternalHyperlinkTextColorOption.value();
	} else {
		return baseStyle.ExternalHyperlinkTextColorOption.value();
	}
}

ZLColor ZLTextFullDecoratedStyle::color() const {
	ZLTextStyleDecoration::HyperlinkStyle hyperlinkStyle = myDecoration.hyperlinkStyle();
	if (hyperlinkStyle == ZLTextStyleDecoration::NONE) {
		return base()->color();
	}
	ZLTextBaseStyle &baseStyle = ZLTextStyleCollection::instance().baseStyle();
	if (hyperlinkStyle == ZLTextStyleDecoration::INTERNAL) {
		return baseStyle.InternalHyperlinkTextColorOption.value();
	} else {
		return baseStyle.ExternalHyperlinkTextColorOption.value();
	}
}

int ZLTextForcedStyle::leftIndent() const {
	return myEntry.leftIndentSupported() ? myEntry.leftIndent() : base()->leftIndent();
}

int ZLTextForcedStyle::rightIndent() const {
	return myEntry.rightIndentSupported() ? myEntry.rightIndent() : base()->rightIndent();
}

ZLTextAlignmentType ZLTextForcedStyle::alignment() const {
	return myEntry.alignmentTypeSupported() ? myEntry.alignmentType() : base()->alignment();
}
