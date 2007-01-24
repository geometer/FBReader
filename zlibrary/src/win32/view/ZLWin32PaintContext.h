/*
 * Copyright (C) 2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __ZLWIN32PAINTCONTEXT_H__
#define __ZLWIN32PAINTCONTEXT_H__

#include <windows.h>

#include <ZLPaintContext.h>

class ZLWin32ApplicationWindow;

class ZLWin32PaintContext : public ZLPaintContext {

public:
	ZLWin32PaintContext();
	~ZLWin32PaintContext();

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
	void drawString(int x, int y, const char *str, int len);

	void drawImage(int x, int y, const ZLImageData &image);

	void drawLine(int x0, int y0, int x1, int y1);
	void fillRectangle(int x0, int y0, int x1, int y1);
	void drawFilledCircle(int x, int y, int r);

	void beginPaint(ZLWin32ApplicationWindow &window);
	void endPaint();

private:
	void adjustPoint(int &x, int &y) const;

private:
	HDC myDisplayContext;
	HWND myWindow;
	int myTopOffset;
	PAINTSTRUCT myPaintStructure;

	bool myColorIsUpToDate;
	LineStyle myLineStyle;
	COLORREF myColorref;

	ZLColor myBackgroundColor;
	HBRUSH myBackgroundBrush;

	HBRUSH myFillBrush;

	int myWidth;
	int myHeight;
	/*
	QPainter *myPainter;
	QPixmap *myPixmap;
	mutable int mySpaceWidth;

	bool myFontIsStored;
	std::string myStoredFamily;
	int myStoredSize;
	bool myStoredBold;
	bool myStoredItalic;	
	*/
};

inline bool operator == (const ZLColor &color, const COLORREF &colorref) {
	return
		(color.Red == (unsigned char)colorref) &&
		(color.Green == (unsigned char)(colorref >> 8)) &&
		(color.Blue == (unsigned char)(colorref >> 16));
}

inline bool operator != (const ZLColor &color, const COLORREF &colorref) {
	return !(color == colorref);
}

inline bool operator == (const COLORREF &colorref, const ZLColor &color) {
	return color == colorref;
}

inline bool operator != (const COLORREF &colorref, const ZLColor &color) {
	return !(colorref == color);
}

#endif /* __ZLWIN32PAINTCONTEXT_H__ */
