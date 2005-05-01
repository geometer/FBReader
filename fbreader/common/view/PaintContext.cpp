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

#include "PaintContext.h"

static const std::string COLORS = "Colors";
static const std::string OPTIONS = "Options";

ZLColorOption PaintContext::BackgroundColorOption(COLORS, "Background", ZLColor(255, 255, 255));

PaintContext::PaintContext() :
			myLeftMargin(OPTIONS, "LeftMargin", 4), myRightMargin(OPTIONS, "RightMargin", 4),
			myTopMargin(OPTIONS, "TopMargin", 0), myBottomMargin(OPTIONS, "BottomMargin", 4) {
}

const std::vector<std::string> &PaintContext::fontFamilies() const {
	if (myFamilies.empty()) {
		fillFamiliesList(myFamilies);
	}
	return myFamilies;
}
