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

#include "FBOptions.h"

FBOptions* FBOptions::ourInstance = 0;

static const std::string OPTIONS = "Options";
static const std::string COLORS = "Colors";

FBOptions::FBOptions() :
	LeftMarginOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "LeftMargin", 0, 1000, 4),
	RightMarginOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "RightMargin", 0, 1000, 4),
	TopMarginOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "TopMargin", 0, 1000, 0),
	BottomMarginOption(ZLCategoryKey::LOOK_AND_FEEL, OPTIONS, "BottomMargin", 0, 1000, 4),
	BackgroundColorOption(ZLCategoryKey::LOOK_AND_FEEL, COLORS, "Background", ZLColor(255, 255, 255)),
	RegularTextColorOption(ZLCategoryKey::LOOK_AND_FEEL, COLORS, "Text", ZLColor(0, 0, 0)) {
	myColorOptions["internal"] = new ZLColorOption(
		ZLCategoryKey::LOOK_AND_FEEL, COLORS,
		"Hyperlink", ZLColor(33, 96, 180)
	);
	myColorOptions["external"] = new ZLColorOption(
		ZLCategoryKey::LOOK_AND_FEEL, COLORS,
		"ExternalHyperlink", ZLColor(33, 96, 180)
	);
	myColorOptions["book"] = new ZLColorOption(
		ZLCategoryKey::LOOK_AND_FEEL, COLORS,
		"BookHyperlink", ZLColor(23, 68, 128)
	);
	myColorOptions[ZLTextStyle::SELECTION_BACKGROUND] = new ZLColorOption(
		ZLCategoryKey::LOOK_AND_FEEL, COLORS,
		"SelectionBackground", ZLColor(82, 131, 194)
	);
	myColorOptions[ZLTextStyle::HIGHLIGHTED_TEXT] = new ZLColorOption(
		ZLCategoryKey::LOOK_AND_FEEL, COLORS,
		"SelectedText", ZLColor(60, 139, 255)
	);
	myColorOptions[ZLTextStyle::TREE_LINES] = new ZLColorOption(
		ZLCategoryKey::LOOK_AND_FEEL, COLORS, 
		"TreeLines", ZLColor(127, 127, 127)
	);
}

ZLColorOption &FBOptions::colorOption(const std::string &style) {
	std::map<std::string,shared_ptr<ZLColorOption> >::const_iterator it =
		myColorOptions.find(style);
	return it != myColorOptions.end() ? *it->second : RegularTextColorOption;
}
