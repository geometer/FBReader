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

#ifndef __ZLTEXTRECTANGLE_H__
#define __ZLTEXTRECTANGLE_H__

#include <vector>

#include <ZLTextStyle.h>
#include <ZLTextElement.h>

struct ZLTextRectangle {
	int XStart, XEnd, YStart, YEnd;
	ZLTextRectangle(int xStart, int xEnd, int yStart, int yEnd);

	struct RangeChecker {
		RangeChecker(int x, int y) : myX(x), myY(y) {}
		bool operator () (const ZLTextRectangle &position) const {
			return
				(myX >= position.XStart) && (myX <= position.XEnd) &&
				(myY >= position.YStart) && (myY <= position.YEnd);
		}

		int myX, myY;
	};
};

struct ZLTextElementRectangle : public ZLTextRectangle {
	int ParagraphIndex;
	int ElementIndex;
	int StartCharIndex;
	int Length;
	bool AddHyphenationSign;
	shared_ptr<ZLTextStyle> Style;
	ZLTextElement::Kind Kind;
	unsigned char BidiLevel;
	ZLTextElementRectangle(int paragraphIndex, int elementIndex, int startCharIndex, int length, bool addHyphenationSign, shared_ptr<ZLTextStyle> style, ZLTextElement::Kind kind, int xStart, int xEnd, int yStart, int yEnd, unsigned char bidiLevel);
};

struct ZLTextTreeNodeRectangle : public ZLTextRectangle {
	int ParagraphIndex;
	ZLTextTreeNodeRectangle(int paragraphIndex, int xStart, int xEnd, int yStart, int yEnd);
};

typedef std::vector<ZLTextTreeNodeRectangle> ZLTextTreeNodeMap;
typedef std::vector<ZLTextElementRectangle> ZLTextElementMap;
typedef ZLTextElementMap::const_iterator ZLTextElementIterator;

inline ZLTextRectangle::ZLTextRectangle(int xStart, int xEnd, int yStart, int yEnd) : XStart(xStart), XEnd(xEnd), YStart(yStart), YEnd(yEnd) {}

inline ZLTextElementRectangle::ZLTextElementRectangle(int paragraphIndex, int elementIndex, int startCharIndex, int length, bool addHyphenationSign, shared_ptr<ZLTextStyle> style, ZLTextElement::Kind kind, int xStart, int xEnd, int yStart, int yEnd, unsigned char bidiLevel) : ZLTextRectangle(xStart, xEnd, yStart, yEnd), ParagraphIndex(paragraphIndex), ElementIndex(elementIndex), StartCharIndex(startCharIndex), Length(length), AddHyphenationSign(addHyphenationSign), Style(style), Kind(kind), BidiLevel(bidiLevel) {}

inline ZLTextTreeNodeRectangle::ZLTextTreeNodeRectangle(int paragraphIndex, int xStart, int xEnd, int yStart, int yEnd) : ZLTextRectangle(xStart, xEnd, yStart, yEnd), ParagraphIndex(paragraphIndex) {}

#endif /* __ZLTEXTRECTANGLE_H__ */
