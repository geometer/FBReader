/*
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

#ifndef __ZLPAINTCONTEXT_H__
#define __ZLPAINTCONTEXT_H__

#include <vector>
#include <string>

#include <abstract/ZLOptions.h>

class ZLImage;

class ZLPaintContext {

public:
	static ZLColorOption BackgroundColorOption;
	
protected:
	ZLPaintContext() PAINT_SECTION;

public:
	virtual ~ZLPaintContext() PAINT_SECTION;
	virtual void removeCaches() PAINT_SECTION;

	int x() const PAINT_SECTION;
	int y() const PAINT_SECTION;

	void moveXTo(int x) PAINT_SECTION;
	void moveX(int deltaX) PAINT_SECTION;
	void moveYTo(int y) PAINT_SECTION;
	void moveY(int deltaY) PAINT_SECTION;

	const ZLIntegerOption &leftMargin() const PAINT_SECTION;;
	const ZLIntegerOption &rightMargin() const PAINT_SECTION;;
	const ZLIntegerOption &topMargin() const PAINT_SECTION;;
	const ZLIntegerOption &bottomMargin() const PAINT_SECTION;;

	virtual void clear() PAINT_SECTION = 0;

	virtual void setFont(const std::string &family, int size, bool bold, bool italic) PAINT_SECTION = 0;
	virtual void setColor(ZLColor color) PAINT_SECTION = 0;
	virtual void setFillColor(ZLColor color) PAINT_SECTION = 0;

	virtual int width() const PAINT_SECTION = 0;
	virtual int height() const PAINT_SECTION = 0;
	
	virtual int stringWidth(const char *str, int len) const PAINT_SECTION = 0;
	virtual int spaceWidth() const PAINT_SECTION = 0;
	virtual int stringHeight() const PAINT_SECTION = 0;
	virtual void drawString(int x, int y, const char *str, int len) PAINT_SECTION = 0;

	virtual int imageWidth(const ZLImage &image) const PAINT_SECTION = 0;
	virtual int imageHeight(const ZLImage &image) const PAINT_SECTION = 0;
	virtual void drawImage(int x, int y, const ZLImage &image) PAINT_SECTION = 0;

	virtual void drawLine(int x0, int y0, int x1, int y1) PAINT_SECTION = 0;
	virtual void fillRectangle(int x0, int y0, int x1, int y1) PAINT_SECTION = 0;

	const std::vector<std::string> &fontFamilies() const PAINT_SECTION;
	virtual const std::string realFontFamilyName(std::string &fontFamily) const PAINT_SECTION = 0;

protected:
	virtual void fillFamiliesList(std::vector<std::string> &families) const PAINT_SECTION = 0;

private:
	int myX, myY;

	ZLIntegerOption myLeftMargin;
	ZLIntegerOption myRightMargin;
	ZLIntegerOption myTopMargin;
	ZLIntegerOption myBottomMargin;

	mutable std::vector<std::string> myFamilies;

private:
	ZLPaintContext(const ZLPaintContext&);
	const ZLPaintContext& operator = (const ZLPaintContext&);
};

inline int ZLPaintContext::x() const { return myX; }
inline int ZLPaintContext::y() const { return myY; }

inline void ZLPaintContext::moveXTo(int x) { myX = x; }
inline void ZLPaintContext::moveX(int deltaX) { myX += deltaX; }
inline void ZLPaintContext::moveYTo(int y) { myY = y; }
inline void ZLPaintContext::moveY(int deltaY) { myY += deltaY; }

inline const ZLIntegerOption &ZLPaintContext::leftMargin() const { return myLeftMargin; };
inline const ZLIntegerOption &ZLPaintContext::rightMargin() const { return myRightMargin; };
inline const ZLIntegerOption &ZLPaintContext::topMargin() const { return myTopMargin; };
inline const ZLIntegerOption &ZLPaintContext::bottomMargin() const { return myBottomMargin; };

#endif /* __ZLPAINTCONTEXT_H__ */
