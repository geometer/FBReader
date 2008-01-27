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

#include "ZLTextStyle.h"

static const std::string COLORS = "Colors";
static const std::string GROUP = "Style";

ZLTextBaseStyle::ZLTextBaseStyle(const std::string &fontFamily, int fontSize) :
	BackgroundColorOption(ZLCategoryKey::LOOK_AND_FEEL, COLORS, "Background", ZLColor(255, 255, 255)),
	SelectionBackgroundColorOption(ZLCategoryKey::LOOK_AND_FEEL, COLORS, "SelectionBackground", ZLColor(82, 131, 194)),
	SelectedTextColorOption(ZLCategoryKey::LOOK_AND_FEEL, COLORS, "SelectedText", ZLColor(60, 139, 255)),
	RegularTextColorOption(ZLCategoryKey::LOOK_AND_FEEL, COLORS, "Text", ZLColor(0, 0, 0)),
	InternalHyperlinkTextColorOption(ZLCategoryKey::LOOK_AND_FEEL, COLORS, "Hyperlink", ZLColor(33, 96, 180)),
	ExternalHyperlinkTextColorOption(ZLCategoryKey::LOOK_AND_FEEL, COLORS, "ExternalHyperlink", ZLColor(98, 174, 26)),
	TreeLinesColorOption(ZLCategoryKey::LOOK_AND_FEEL, COLORS, "TreeLines", ZLColor(127, 127, 127)),
	AutoHyphenationOption(ZLCategoryKey::LOOK_AND_FEEL, "Options", "AutoHyphenation", true),
	FontFamilyOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP, "Base:fontFamily", fontFamily),
	FontSizeOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP, "Base:fontSize", 0, 72, fontSize),
	BoldOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP, "Base:bold", false),
	ItalicOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP, "Base:italic", false),
	AlignmentOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP, "Base:alignment", ALIGN_JUSTIFY),
	LineSpaceOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP, "Base:lineSpacing", 1.2),
	LineSpacePercentOption(ZLCategoryKey::LOOK_AND_FEEL, GROUP, "Base:lineSpacingPercent", 120) {
}

ZLColor ZLTextBaseStyle::color() const {
	return RegularTextColorOption.value();
}
