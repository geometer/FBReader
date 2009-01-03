/*
 * Copyright (C) 2004-2009 Geometer Plus <contact@geometerplus.com>
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

struct ZLTextRectangularArea {
	int XStart, XEnd, YStart, YEnd;
	ZLTextRectangularArea(int xStart, int xEnd, int yStart, int yEnd);

	struct RangeChecker {
		RangeChecker(int x, int y) : myX(x), myY(y) {}
		bool operator()(const ZLTextRectangularArea &position) const {
			return
				(myX >= position.XStart) && (myX <= position.XEnd) &&
				(myY >= position.YStart) && (myY <= position.YEnd);
		}

		int myX, myY;
	};
};

struct ZLTextElementArea : public ZLTextRectangularArea {
	int ParagraphIndex;
	int ElementIndex;
	int StartCharIndex;
	int Length;
	bool AddHyphenationSign;
	ZLTextStylePtr Style;
	ZLTextElement::Kind Kind;
	unsigned char BidiLevel;
	ZLTextElementArea(int paragraphIndex, int elementIndex, int startCharIndex, int length, bool addHyphenationSign, ZLTextStylePtr style, ZLTextElement::Kind kind, int xStart, int xEnd, int yStart, int yEnd, unsigned char bidiLevel);
};

struct ZLTextTreeNodeArea : public ZLTextRectangularArea {
	int ParagraphIndex;
	ZLTextTreeNodeArea(int paragraphIndex, int xStart, int xEnd, int yStart, int yEnd);
};

typedef std::vector<ZLTextTreeNodeArea> ZLTextTreeNodeMap;
typedef std::vector<ZLTextElementArea> ZLTextElementMap;
typedef ZLTextElementMap::const_iterator ZLTextElementIterator;

inline ZLTextRectangularArea::ZLTextRectangularArea(int xStart, int xEnd, int yStart, int yEnd) : XStart(xStart), XEnd(xEnd), YStart(yStart), YEnd(yEnd) {}

inline ZLTextElementArea::ZLTextElementArea(int paragraphIndex, int elementIndex, int startCharIndex, int length, bool addHyphenationSign, ZLTextStylePtr style, ZLTextElement::Kind kind, int xStart, int xEnd, int yStart, int yEnd, unsigned char bidiLevel) : ZLTextRectangularArea(xStart, xEnd, yStart, yEnd), ParagraphIndex(paragraphIndex), ElementIndex(elementIndex), StartCharIndex(startCharIndex), Length(length), AddHyphenationSign(addHyphenationSign), Style(style), Kind(kind), BidiLevel(bidiLevel) {}

inline ZLTextTreeNodeArea::ZLTextTreeNodeArea(int paragraphIndex, int xStart, int xEnd, int yStart, int yEnd) : ZLTextRectangularArea(xStart, xEnd, yStart, yEnd), ParagraphIndex(paragraphIndex) {}

#endif /* __ZLTEXTAREA_H__ */
