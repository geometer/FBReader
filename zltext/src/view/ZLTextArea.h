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

#ifndef __ZLTEXTAREA_H__
#define __ZLTEXTAREA_H__

#include <vector>

#include "ZLTextElement.h"
#include <ZLTextStyle.h>

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
	int StartCharNumber;
	int Length;
	bool AddHyphenationSign;
	bool ChangeStyle;
	ZLTextStylePtr Style;
	TextElement::Kind Kind;
	TextElementArea(int paragraphNumber, int textElementNumber, int startCharNumber, int length, bool addHyphenationSign, bool changeStyle, ZLTextStylePtr style, TextElement::Kind kind, int xStart, int xEnd, int yStart, int yEnd);
};

struct TreeNodeArea : public RectangularArea {
	int ParagraphNumber;
	TreeNodeArea(int paragraphNumber, int xStart, int xEnd, int yStart, int yEnd);
};

typedef std::vector<TreeNodeArea> TreeNodeMap;
typedef std::vector<TextElementArea> TextElementMap;
typedef TextElementMap::const_iterator TextElementIterator;

inline RectangularArea::RectangularArea(int xStart, int xEnd, int yStart, int yEnd) : XStart(xStart), XEnd(xEnd), YStart(yStart), YEnd(yEnd) {}

inline TextElementArea::TextElementArea(int paragraphNumber, int textElementNumber, int startCharNumber, int length, bool addHyphenationSign, bool changeStyle, ZLTextStylePtr style, TextElement::Kind kind, int xStart, int xEnd, int yStart, int yEnd) : RectangularArea(xStart, xEnd, yStart, yEnd), ParagraphNumber(paragraphNumber), TextElementNumber(textElementNumber), StartCharNumber(startCharNumber), Length(length), AddHyphenationSign(addHyphenationSign), ChangeStyle(changeStyle), Style(style), Kind(kind) {}

inline TreeNodeArea::TreeNodeArea(int paragraphNumber, int xStart, int xEnd, int yStart, int yEnd) : RectangularArea(xStart, xEnd, yStart, yEnd), ParagraphNumber(paragraphNumber) {}

#endif /* __ZLTEXTAREA_H__ */
