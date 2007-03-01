/*
 * FBReader -- electronic book reader
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

#include <ZLOptions.h>

#include "TextStyle.h"

static const std::string COLORS = "Colors";
static const std::string OPTIONS = "Options";
static const std::string GROUP = "Style";

BaseTextStyle::BaseTextStyle(const std::string &fontFamily, int fontSize) :
	LeftMarginOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "LeftMargin", 0, 100, 4),
	RightMarginOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "RightMargin", 0, 100, 4),
	TopMarginOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "TopMargin", 0, 100, 0),
	BottomMarginOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "BottomMargin", 0, 100, 4),
	BackgroundColorOption(ZLOption::LOOK_AND_FEEL_CATEGORY, COLORS, "Background", ZLColor(255, 255, 255)),
	SelectedTextColorOption(ZLOption::LOOK_AND_FEEL_CATEGORY, COLORS, "SelectedText", ZLColor(0, 0, 127)),
	RegularTextColorOption(ZLOption::LOOK_AND_FEEL_CATEGORY, COLORS, "Text", ZLColor(0, 0, 0)),
	InternalHyperlinkTextColorOption(ZLOption::LOOK_AND_FEEL_CATEGORY, COLORS, "Hyperlink", ZLColor(63, 63, 127)),
	ExternalHyperlinkTextColorOption(ZLOption::LOOK_AND_FEEL_CATEGORY, COLORS, "ExternalHyperlink", ZLColor(63, 127, 63)),
	TreeLinesColorOption(ZLOption::LOOK_AND_FEEL_CATEGORY, COLORS, "TreeLines", ZLColor(127, 127, 127)),
	AutoHyphenationOption(ZLOption::LOOK_AND_FEEL_CATEGORY, OPTIONS, "AutoHyphenation", true),
	FontFamilyOption(ZLOption::LOOK_AND_FEEL_CATEGORY, GROUP, "Base:fontFamily", fontFamily),
	FontSizeOption(ZLOption::LOOK_AND_FEEL_CATEGORY, GROUP, "Base:fontSize", 0, 72, fontSize),
	BoldOption(ZLOption::LOOK_AND_FEEL_CATEGORY, GROUP, "Base:bold", false),
	ItalicOption(ZLOption::LOOK_AND_FEEL_CATEGORY, GROUP, "Base:italic", false),
	AlignmentOption(ZLOption::LOOK_AND_FEEL_CATEGORY, GROUP, "Base:alignment", ALIGN_JUSTIFY),
	LineSpaceOption(ZLOption::LOOK_AND_FEEL_CATEGORY, GROUP, "Base:lineSpacing", 1.3) {
}

ZLColor BaseTextStyle::color() const {
	return RegularTextColorOption.value();
}
