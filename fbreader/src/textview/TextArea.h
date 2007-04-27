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

#ifndef __TEXTAREA_H__
#define __TEXTAREA_H__

#include <vector>

#include "TextElement.h"

struct RectangularArea {
	int XStart, XEnd, YStart, YEnd;
	RectangularArea(int xStart, int xEnd, int yStart, int yEnd);

	struct RangeChecker {
		RangeChecker(int x, int y) : myX(x), myY(y) {}
		bool operator()(const RectangularArea &position) const {
			return
				(myX >= position.XStart) && (myX <= position.XEnd) &&
				(myY >= position.YStart) && (myY <= position.YEnd);
		}

		int myX, myY;
	};
};

struct TextElementArea : public RectangularArea {
	int ParagraphNumber;
	int TextElementNumber;
	/*
	 * if CharacterNumber == N 
	 *   area contains first N characters of the element
	 * if CharacterNumber == -N 
	 *   area contains last N characters of the element
	 * if CharacterNumber == 0 
	 *   area contains full element
	 */
	int CharacterNumber;
	TextElement::Kind Kind;
	TextElementArea(int paragraphNumber, int textElementNumber, int characterNumber, TextElement::Kind kind, int xStart, int xEnd, int yStart, int yEnd);
};

struct TreeNodeArea : public RectangularArea {
	int ParagraphNumber;
	TreeNodeArea(int paragraphNumber, int xStart, int xEnd, int yStart, int yEnd);
};

typedef std::vector<TreeNodeArea> TreeNodeMap;
typedef std::vector<TextElementArea> TextElementMap;

inline RectangularArea::RectangularArea(int xStart, int xEnd, int yStart, int yEnd) : XStart(xStart), XEnd(xEnd), YStart(yStart), YEnd(yEnd) {}

inline TextElementArea::TextElementArea(int paragraphNumber, int textElementNumber, int characterNumber, TextElement::Kind kind, int xStart, int xEnd, int yStart, int yEnd) : RectangularArea(xStart, xEnd, yStart, yEnd), ParagraphNumber(paragraphNumber), TextElementNumber(textElementNumber), CharacterNumber(characterNumber), Kind(kind) {}

inline TreeNodeArea::TreeNodeArea(int paragraphNumber, int xStart, int xEnd, int yStart, int yEnd) : RectangularArea(xStart, xEnd, yStart, yEnd), ParagraphNumber(paragraphNumber) {}

#endif /* __TEXTAREA_H__ */
