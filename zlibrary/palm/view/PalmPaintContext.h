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
	PalmPaintContext() ZLVIEW_SECTION;
	~PalmPaintContext() ZLVIEW_SECTION;
	void removeCaches() ZLVIEW_SECTION;

	void setSize(int w, int h) ZLVIEW_SECTION;

	int width() const ZLVIEW_SECTION;
	int height() const ZLVIEW_SECTION;

	void clear(ZLColor color) ZLVIEW_SECTION;

	void fillFamiliesList(std::vector<std::string> &families) const ZLVIEW_SECTION;
	const std::string realFontFamilyName(std::string &fontFamily) const ZLVIEW_SECTION;

	void setFont(const std::string &family, int size, bool bold, bool italic) ZLVIEW_SECTION;
	void setColor(ZLColor color, LineStyle style = SOLID_LINE) ZLVIEW_SECTION;
	void setFillColor(ZLColor color, FillStyle style = SOLID_FILL) ZLVIEW_SECTION;

	int stringWidth(const char *str, int len) const ZLVIEW_SECTION;
	int spaceWidth() const ZLVIEW_SECTION;
	int stringHeight() const ZLVIEW_SECTION;
	void drawString(int x, int y, const char *str, int len) ZLVIEW_SECTION;

	int imageWidth(const ZLImage &image) const ZLVIEW_SECTION;
	int imageHeight(const ZLImage &image) const ZLVIEW_SECTION;
	void drawImage(int x, int y, const ZLImage &image) ZLVIEW_SECTION;

	void drawLine(int x0, int y0, int x1, int y1) ZLVIEW_SECTION;
	void fillRectangle(int x0, int y0, int x1, int y1) ZLVIEW_SECTION;
	void drawFilledCircle(int x, int y, int r) ZLVIEW_SECTION;

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
