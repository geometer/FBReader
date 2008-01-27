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

#include <ZLImage.h>
#include <ZLUnicodeUtil.h>

#include "ZLWin32PaintContext.h"
#include "../application/ZLWin32ApplicationWindow.h"
#include "../image/ZLWin32ImageManager.h"
#include "../../../../core/src/win32/util/W32WCHARUtil.h"

ZLWin32PaintContext::ZLWin32PaintContext() : myDisplayContext(0), myBufferBitmap(0), myWidth(0), myHeight(0), myBackgroundBrush(0), myFillBrush(0), mySpaceWidth(-1) {
}

ZLWin32PaintContext::~ZLWin32PaintContext() {
	if (myBackgroundBrush != 0) {
		DeleteObject(myBackgroundBrush);
	}
	if (myFillBrush != 0) {
		DeleteObject(myFillBrush);
	}
	if (myBufferBitmap != 0) {
		DeleteObject(myBufferBitmap);
		DeleteDC(myDisplayContext);
	}
}

void ZLWin32PaintContext::updateInfo(HWND window, int width, int height) {
	if (myBufferBitmap != 0) {
		if ((myWidth != width) || (myHeight != height)) {
			DeleteObject(myBufferBitmap);
			DeleteDC(myDisplayContext);
			myBufferBitmap = 0;
			myColorIsUpToDate = false;
		}
	}

	if (myBufferBitmap == 0) {
		myWidth = width;
		myHeight = height;
		HDC dc = GetDC(window);
		myDisplayContext = CreateCompatibleDC(dc);
		myBufferBitmap = CreateCompatibleBitmap(dc, myWidth + 1, myHeight + 1);
		ReleaseDC(window, dc);
		SelectObject(myDisplayContext, myBufferBitmap);
	}
}

static int CALLBACK enumFamiliesProc(const LOGFONTA *lf, const TEXTMETRICA*, DWORD, LPARAM param) {
	((std::set<std::string>*)param)->insert(lf->lfFaceName);
	return 1;
}

void ZLWin32PaintContext::fillFamiliesList(std::vector<std::string> &families) const {
	std::set<std::string> familiesSet;
	LOGFONTA font;
	font.lfCharSet = DEFAULT_CHARSET;
	font.lfFaceName[0] = '\0';
	font.lfPitchAndFamily = 0;
	HDC dc = GetDC(0);
	EnumFontFamiliesExA(dc, &font, enumFamiliesProc, (LPARAM)&familiesSet, 0);
	ReleaseDC(0, dc);
	for (std::set<std::string>::const_iterator it = familiesSet.begin(); it != familiesSet.end(); ++it) {
		families.push_back(*it);
	}
}

const std::string ZLWin32PaintContext::realFontFamilyName(std::string &fontFamily) const {
	/*
	QString fullName = QFontInfo(QFont(fontFamily.c_str())).family();
	if (fullName.isNull() || fullName.isEmpty()) {
		fullName = QFontInfo(QFont::defaultFont()).family();
		if (fullName.isNull() || fullName.isEmpty()) {
			return HELVETICA;
		}
	}
	return fullName.left(fullName.find(" [")).ascii();
	*/
	return fontFamily;
}

void ZLWin32PaintContext::setFont(const std::string &family, int size, bool bold, bool italic) {
	if (myDisplayContext == 0) {
		return;
	}
	// TODO: optimize
	LOGFONT logicalFont;
	memset(&logicalFont, 0, sizeof(LOGFONT));
	logicalFont.lfHeight = size;
	logicalFont.lfWeight = bold ? FW_BOLD : FW_REGULAR;
	logicalFont.lfItalic = italic;
	logicalFont.lfQuality = 5 /*CLEARTYPE_QUALITY*/;
	const int len = std::min((int)family.length(), LF_FACESIZE - 1);
	ZLUnicodeUtil::Ucs2String str;
	ZLUnicodeUtil::utf8ToUcs2(str, family.data(), len);
	str.push_back(0);
	memcpy(logicalFont.lfFaceName, ::wchar(str), 2 * str.size());
	HFONT font = CreateFontIndirect(&logicalFont);
	DeleteObject(SelectObject(myDisplayContext, font));

	GetTextMetrics(myDisplayContext, &myTextMetric);
	mySpaceWidth = -1;
}

void ZLWin32PaintContext::setColor(ZLColor color, LineStyle style) {
	if (myDisplayContext == 0) {
		return;
	}
	if (!myColorIsUpToDate || (color != myColor) || (style != myLineStyle)) {
		myColorIsUpToDate = false;
		myLineStyle = style;
		myColor = RGB(color.Red, color.Green, color.Blue);
		SetTextColor(myDisplayContext, myColor);
		DeleteObject(SelectObject(myDisplayContext, CreatePen((style == ZLPaintContext::SOLID_LINE) ? PS_SOLID : PS_DASH, 1, myColor)));
	}
}

void ZLWin32PaintContext::setFillColor(ZLColor color, FillStyle style) {
	if (myDisplayContext == 0) {
		return;
	}
	// TODO: optimize (don't create new brush, if color and style are not changed)
	if (myFillBrush != 0) {
		DeleteObject(myFillBrush);
	}
	COLORREF colorref = RGB(color.Red, color.Green, color.Blue);
	myFillBrush =
		(style == SOLID_FILL) ?
			CreateSolidBrush(colorref) :
			CreateHatchBrush(HS_DIAGCROSS, colorref);
}

int ZLWin32PaintContext::stringWidth(const char *str, int len) const {
	if (myDisplayContext == 0) {
		return 0;
	}
	SIZE size;
	int utf8len = ZLUnicodeUtil::utf8Length(str, len);
	if (utf8len == len) {
		GetTextExtentPointA(myDisplayContext, str, len, &size);
	} else {
		static ZLUnicodeUtil::Ucs2String ucs2Str;
		ucs2Str.clear();
		ZLUnicodeUtil::utf8ToUcs2(ucs2Str, str, len, utf8len);
		GetTextExtentPointW(myDisplayContext, ::wchar(ucs2Str), utf8len, &size);
	}
	return size.cx;
}

int ZLWin32PaintContext::spaceWidth() const {
	if (mySpaceWidth == -1) {
		GetCharWidth(myDisplayContext, ' ', ' ', &mySpaceWidth);
	}
	return mySpaceWidth;
}

int ZLWin32PaintContext::stringHeight() const {
	return myTextMetric.tmHeight;
}

int ZLWin32PaintContext::descent() const {
	return myTextMetric.tmDescent;
}

void ZLWin32PaintContext::drawString(int x, int y, const char *str, int len) {
	if (myDisplayContext == 0) {
		return;
	}
	y -= stringHeight();
	y += myTextMetric.tmDescent;
	int utf8len = ZLUnicodeUtil::utf8Length(str, len);
	if (utf8len == len) {
		TextOutA(myDisplayContext, x, y, str, len);
	} else {
		static ZLUnicodeUtil::Ucs2String ucs2Str;
		ucs2Str.clear();
		ZLUnicodeUtil::utf8ToUcs2(ucs2Str, str, len, utf8len);
		TextOutW(myDisplayContext, x, y, ::wchar(ucs2Str), utf8len);
	}
}

void ZLWin32PaintContext::drawImage(int x, int y, const ZLImageData &image) {
	ZLWin32ImageData &win32Image = (ZLWin32ImageData&)image;
	const BYTE *pixels = win32Image.pixels(myBackgroundColor);
	if (pixels != 0) {
		const int width = image.width();
		const int height = image.height();
		StretchDIBits(myDisplayContext,
			x, y - height, width, height,
			0, 0, width, height,
			pixels, win32Image.info(), DIB_RGB_COLORS, SRCCOPY);
	}
}

void ZLWin32PaintContext::drawLine(int x0, int y0, int x1, int y1) {
	if (myDisplayContext == 0) {
		return;
	}

	MoveToEx(myDisplayContext, x0, y0, 0);
	LineTo(myDisplayContext, x1, y1);
	SetPixel(myDisplayContext, x0, y0, myColor);
	SetPixel(myDisplayContext, x1, y1, myColor);
}

void ZLWin32PaintContext::fillRectangle(int x0, int y0, int x1, int y1) {
	if (myDisplayContext == 0) {
		return;
	}

	RECT rectangle;
	if (x0 < x1) {
		rectangle.left = x0;
		rectangle.right = x1 + 1;
	} else {
		rectangle.left = x1;
		rectangle.right = x0 + 1;
	}
	if (y0 < y1) {
		rectangle.top = y0;
		rectangle.bottom = y1 + 1;
	} else {
		rectangle.top = y1;
		rectangle.bottom = y0 + 1;
	}
	FillRect(myDisplayContext, &rectangle, myFillBrush);
}

void ZLWin32PaintContext::drawFilledCircle(int x, int y, int r) {
	if (myDisplayContext == 0) {
		return;
	}

	HBRUSH oldBrush = (HBRUSH)SelectObject(myDisplayContext, myFillBrush);
	Ellipse(myDisplayContext, x - r, y - r, x + r, y + r);
	SelectObject(myDisplayContext, oldBrush);
}

void ZLWin32PaintContext::clear(ZLColor color) {
	if (myDisplayContext == 0) {
		return;
	}
	if ((myBackgroundBrush == 0) || (color != myBackgroundColor)) {
		if (myBackgroundBrush != 0) {
			DeleteObject(myBackgroundBrush);
		}
		myBackgroundColor = color;
		myBackgroundBrush = CreateSolidBrush(RGB(color.Red, color.Green, color.Blue));
	}
	RECT rectangle;
	rectangle.top = 0;
	rectangle.bottom = myHeight;
	rectangle.left = 0;
	rectangle.right = myWidth;
	FillRect(myDisplayContext, &rectangle, myBackgroundBrush);
	SetBkMode(myDisplayContext, TRANSPARENT);
}

int ZLWin32PaintContext::width() const {
	return myWidth;
}

int ZLWin32PaintContext::height() const {
	return myHeight;
}

HDC ZLWin32PaintContext::displayContext() const {
	return myDisplayContext;
}

HBITMAP ZLWin32PaintContext::buffer() const {
	return myBufferBitmap;
}
