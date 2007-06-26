/*
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

#include "ZLPaintContext.h"

ZLPaintContext::ZLPaintContext() : myLeftMargin(0), myRightMargin(0), myTopMargin(0), myBottomMargin(0) {
}

ZLPaintContext::~ZLPaintContext() {
}

const std::vector<std::string> &ZLPaintContext::fontFamilies() const {
	if (myFamilies.empty()) {
		fillFamiliesList(myFamilies);
	}
	return myFamilies;
}

void ZLPaintContext::setLeftMargin(int margin) {
	if (width() + myLeftMargin - margin > 0) {
		myLeftMargin = margin;
	}
}

void ZLPaintContext::setRightMargin(int margin) {
	if (width() + myRightMargin - margin > 0) {
		myRightMargin = margin;
	}
}

void ZLPaintContext::setTopMargin(int margin) {
	if (height() + myTopMargin - margin > 0) {
		myTopMargin = margin;
	}
}

void ZLPaintContext::setBottomMargin(int margin) {
	if (height() + myBottomMargin - margin > 0) {
		myBottomMargin = margin;
	}
}
