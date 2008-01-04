/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLQTPAINTCONTEXT_H__
#define __ZLQTPAINTCONTEXT_H__

#include <ZLPaintContext.h>

class QPainter;
class QPixmap;
class QImage;

class ZLQtPaintContext : public ZLPaintContext {

public:
	ZLQtPaintContext();
	~ZLQtPaintContext();

	const QPixmap &pixmap() const { return *myPixmap; }

	void setSize(int w, int h);

	int width() const;
	int height() const;

	void clear(ZLColor color);

	void fillFamiliesList(std::vector<std::string> &families) const;
	const std::string realFontFamilyName(std::string &fontFamily) const;

	void setFont(const std::string &family, int size, bool bold, bool italic);
	void setColor(ZLColor color, LineStyle style = SOLID_LINE);
	void setFillColor(ZLColor color, FillStyle style = SOLID_FILL);

	int stringWidth(const char *str, int len) const;
	int spaceWidth() const;
	int stringHeight() const;
	int descent() const;
	void drawString(int x, int y, const char *str, int len);

	void drawImage(int x, int y, const ZLImageData &image);

	void drawLine(int x0, int y0, int x1, int y1);
	void fillRectangle(int x0, int y0, int x1, int y1);
	void drawFilledCircle(int x, int y, int r);

private:
	QPainter *myPainter;
	QPixmap *myPixmap;
	mutable int mySpaceWidth;
	int myDescent;

	bool myFontIsStored;
	std::string myStoredFamily;
	int myStoredSize;
	bool myStoredBold;
	bool myStoredItalic;	
};

#endif /* __ZLQTPAINTCONTEXT_H__ */
