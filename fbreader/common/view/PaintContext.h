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

#ifndef __PAINTCONTEXT_H__
#define __PAINTCONTEXT_H__

#include <vector>
#include <string>

#include <abstract/ZLOptions.h>

class Word;
class Image;

class PaintContext {

public:
	static ZLColorOption BackgroundColorOption;
	
protected:
	PaintContext() VIEW_SECTION;

public:
	virtual ~PaintContext() {}
	virtual void removeCaches() {}

	int x() const { return myX; }
	int y() const { return myY; }

	void moveXTo(int x) { myX = x; }
	void moveX(int deltaX) { myX += deltaX; }
	void moveYTo(int y) { myY = y; }
	void moveY(int deltaY) { myY += deltaY; }

	const ZLIntegerOption &leftMargin() const { return myLeftMargin; };
	const ZLIntegerOption &rightMargin() const { return myRightMargin; };
	const ZLIntegerOption &topMargin() const { return myTopMargin; };
	const ZLIntegerOption &bottomMargin() const { return myBottomMargin; };

	virtual void clear() VIEW_SECTION = 0;

	virtual void setFont(const std::string &family, int size, bool bold, bool italic) VIEW_SECTION = 0;
	virtual void setColor(ZLColor color) VIEW_SECTION = 0;
	virtual void setFillColor(ZLColor color) VIEW_SECTION = 0;

	virtual int width() const VIEW_SECTION = 0;
	virtual int height() const VIEW_SECTION = 0;
	
	virtual int stringWidth(const std::string &str, int from, int len) const VIEW_SECTION = 0;
	virtual int stringHeight() const VIEW_SECTION = 0;
	virtual void drawString(int x, int y, const std::string &str, int from, int len) VIEW_SECTION = 0;
	//virtual void drawWord(int x, int y, const Word &word, int start = 0, int length = -1, bool addHyphenationSign = false) VIEW_SECTION = 0;

	virtual int imageWidth(const Image &image) const VIEW_SECTION = 0;
	virtual int imageHeight(const Image &image) const VIEW_SECTION = 0;
	virtual void drawImage(int x, int y, const Image &image) VIEW_SECTION = 0;

	virtual void drawLine(int x0, int y0, int x1, int y1) VIEW_SECTION = 0;
	virtual void fillRectangle(int x0, int y0, int x1, int y1) VIEW_SECTION = 0;

	const std::vector<std::string> &fontFamilies() const VIEW_SECTION;
	virtual const std::string realFontFamilyName(std::string &fontFamily) const VIEW_SECTION = 0;

protected:
	virtual void fillFamiliesList(std::vector<std::string> &families) const VIEW_SECTION = 0;

private:
	int myX, myY;

	ZLIntegerOption myLeftMargin;
	ZLIntegerOption myRightMargin;
	ZLIntegerOption myTopMargin;
	ZLIntegerOption myBottomMargin;

	mutable std::vector<std::string> myFamilies;

private:
	PaintContext(const PaintContext&);
	const PaintContext& operator = (const PaintContext&);
};

#endif /* __PAINTCONTEXT_H__ */
