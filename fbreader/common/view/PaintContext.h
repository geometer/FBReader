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
	static ZLColorOption TextColorOption;
	static ZLColorOption SelectedTextColorOption;
	
protected:
	PaintContext();

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

	virtual void clear() = 0;

	virtual void setFont(const std::string &family, int size, bool bold, bool italic) = 0;
	virtual void setColor(ZLColor color) = 0;
	virtual void setFillColor(ZLColor color) = 0;

	virtual int width() const = 0;
	virtual int height() const = 0;
	
	virtual int wordWidth(const Word &word, int start = 0, int length = -1, bool addHyphenationSign = false) const = 0;
	virtual int spaceWidth() const = 0;
	virtual int wordHeight() const = 0;
	virtual void drawWord(int x, int y, const Word &word, int start = 0, int length = -1, bool addHyphenationSign = false) = 0;

	virtual int imageWidth(const Image &image) const = 0;
	virtual int imageHeight(const Image &image) const = 0;
	virtual void drawImage(int x, int y, const Image &image) = 0;

	virtual void drawLine(int x0, int y0, int x1, int y1) = 0;
	virtual void fillRectangle(int x0, int y0, int x1, int y1) = 0;

	const std::vector<std::string> &fontFamilies() const;
	virtual const std::string realFontFamilyName(std::string &fontFamily) const = 0;

protected:
	virtual void fillFamiliesList(std::vector<std::string> &families) const = 0;

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
