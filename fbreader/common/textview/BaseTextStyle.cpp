/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

BaseTextStyle::BaseTextStyle(const std::string &fontFamily, int fontSize) {
	std::string group = "Style";

	myFontFamilyOption = new ZLStringOption(group, "Base:fontFamily", fontFamily);
	myFontSizeOption = new ZLIntegerRangeOption(group, "Base:fontSize", 0, 32, fontSize);
	myBoldOption = new ZLBooleanOption(group, "Base:bold", false);
	myItalicOption = new ZLBooleanOption(group, "Base:italic", false);
	myAlignmentOption = new ZLIntegerOption(group, "Base:alignment", ALIGN_JUSTIFY);
	myLineSpaceOption = new ZLDoubleOption(group, "Base:lineSpacing", 1.0);
}

BaseTextStyle::~BaseTextStyle() {
	delete myFontFamilyOption;
	delete myFontSizeOption;
	delete myBoldOption;
	delete myItalicOption;
	delete myAlignmentOption;
	delete myLineSpaceOption;
}

ZLColor BaseTextStyle::color() const {
	return RegularTextColorOption.value();
}
