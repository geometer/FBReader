/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __PALMPAINTCONTEXT_H__
#define __PALMQPAINTCONTEXT_H__

#include <PalmOS.h>				

//#include <map>

#include <abstract/ZLPaintContext.h>

class PalmPaintContext : public ZLPaintContext {

public:
	PalmPaintContext() PAINT_SECTION;
	~PalmPaintContext() PAINT_SECTION;
	void removeCaches() PAINT_SECTION;

	void setSize(int w, int h) PAINT_SECTION;

	int width() const PAINT_SECTION;
	int height() const PAINT_SECTION;

	void clear(ZLColor color) PAINT_SECTION;

	void fillFamiliesList(std::vector<std::string> &families) const PAINT_SECTION;
	const std::string realFontFamilyName(std::string &fontFamily) const PAINT_SECTION;

	void setFont(const std::string &family, int size, bool bold, bool italic) PAINT_SECTION;
	void setColor(ZLColor color, LineStyle style = SOLID_LINE) PAINT_SECTION;
	void setFillColor(ZLColor color, FillStyle style = SOLID_FILL) PAINT_SECTION;

	int stringWidth(const char *str, int len) const PAINT_SECTION;
	int spaceWidth() const PAINT_SECTION;
	int stringHeight() const PAINT_SECTION;
	void drawString(int x, int y, const char *str, int len) PAINT_SECTION;

	int imageWidth(const ZLImage &image) const PAINT_SECTION;
	int imageHeight(const ZLImage &image) const PAINT_SECTION;
	void drawImage(int x, int y, const ZLImage &image) PAINT_SECTION;

	void drawLine(int x0, int y0, int x1, int y1) PAINT_SECTION;
	void fillRectangle(int x0, int y0, int x1, int y1) PAINT_SECTION;
	void drawFilledCircle(int x, int y, int r) PAINT_SECTION;

private:
	//QImage &qImage(const Image &image) const;

private:
	//mutable std::map<const Image*,QImage*> myImageCache;
	int myWidth, myHeight;
	RGBColorType myFillColor;
	mutable int mySpaceWidth;
};

extern bool DO_PAINT;

#endif /* __PALMPAINTCONTEXT_H__ */
