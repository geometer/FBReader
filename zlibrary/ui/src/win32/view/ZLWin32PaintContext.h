/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

	void updateInfo(HWND window, int width, int height);

	HDC displayContext() const;
	HBITMAP buffer() const;

private:
	HDC myDisplayContext;
	HBITMAP myBufferBitmap;
	int myWidth;
	int myHeight;

	bool myColorIsUpToDate;
	LineStyle myLineStyle;
	COLORREF myColor;

	ZLColor myBackgroundColor;
	HBRUSH myBackgroundBrush;

	HBRUSH myFillBrush;

	mutable int mySpaceWidth;

	TEXTMETRIC myTextMetric;
};

inline bool operator == (const ZLColor &color, const COLORREF &colorref) {
	return
		(color.Red == GetRValue(colorref)) &&
		(color.Green == GetGValue(colorref)) &&
		(color.Blue == GetBValue(colorref));
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
