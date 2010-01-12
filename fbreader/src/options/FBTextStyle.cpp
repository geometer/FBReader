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

#include <ZLOptions.h>

#include "FBTextStyle.h"

shared_ptr<ZLTextStyle> FBTextStyle::ourInstance;

shared_ptr<ZLTextStyle> FBTextStyle::InstanceAsPtr() {
	if (ourInstance.isNull()) {
		ourInstance = new FBTextStyle();
	}
	return ourInstance;
}

FBTextStyle &FBTextStyle::Instance() {
	return (FBTextStyle&)*InstanceAsPtr();
}

static const std::string GROUP = "Style";

FBTextStyle::FBTextStyle() :
	FontFamilyOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP, "Base:fontFamily", ""),
	FontSizeOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP, "Base:fontSize", 5, 72, 26),
	BoldOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP, "Base:bold", false),
	ItalicOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP, "Base:italic", false),
	AlignmentOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP, "Base:alignment", ALIGN_JUSTIFY),
	LineSpaceOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP, "Base:lineSpacing", 1.4),
	LineSpacePercentOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP, "Base:lineSpacingPercent", 140) {
}

const std::string &FBTextStyle::colorStyle() const {
	return REGULAR_TEXT;
}

bool FBTextStyle::isDecorated() const {
	return false;
}

const std::string &FBTextStyle::fontFamily() const {
	return FontFamilyOption.value();
}

int FBTextStyle::fontSize() const {
	return FontSizeOption.value();
}

bool FBTextStyle::bold() const {
	return BoldOption.value();
}

bool FBTextStyle::italic() const {
	return ItalicOption.value();
}

short FBTextStyle::spaceBefore(const ZLTextStyleEntry::Metrics&) const {
	return 0;
}

short FBTextStyle::spaceAfter(const ZLTextStyleEntry::Metrics&) const {
	return 0;
}

short FBTextStyle::lineStartIndent(const ZLTextStyleEntry::Metrics&, bool) const {
	return 0;
}

short FBTextStyle::lineEndIndent(const ZLTextStyleEntry::Metrics&, bool) const {
	return 0;
}

short FBTextStyle::firstLineIndentDelta(const ZLTextStyleEntry::Metrics&) const {
	return 0;
}

int FBTextStyle::verticalShift() const {
	return 0;
}

ZLTextAlignmentType FBTextStyle::alignment() const {
	return (ZLTextAlignmentType)AlignmentOption.value();
}

double FBTextStyle::lineSpace() const {
	return LineSpacePercentOption.value() / 100.0;
}

bool FBTextStyle::allowHyphenations() const {
	return true;
}
