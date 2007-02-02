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

#include <ZLImage.h>
#include <ZLUnicodeUtil.h>

#include "ZLWin32PaintContext.h"
#include "../application/ZLWin32ApplicationWindow.h"
#include "../image/ZLWin32ImageManager.h"

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
		myBufferBitmap = CreateCompatibleBitmap(dc, myWidth, myHeight);
		ReleaseDC(window, dc);
		SelectObject(myDisplayContext, myBufferBitmap);
	}
}

void ZLWin32PaintContext::fillFamiliesList(std::vector<std::string> &families) const {
	/*
	QFontDatabase db;
	QStringList qFamilies = db.families();
	bool helveticaFlag = false;
	for (QStringList::Iterator it = qFamilies.begin(); it != qFamilies.end(); ++it) {
		std::string family = (*it).ascii();
		if (family == HELVETICA) {
			helveticaFlag = true;
		}
		families.push_back(family);
	}
	if (!helveticaFlag) {
		families.push_back(HELVETICA);
	}
	*/
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
	const int len = std::min((int)family.size(), LF_FACESIZE - 1);
	strncpy(logicalFont.lfFaceName, family.data(), len);
	logicalFont.lfFaceName[len] = '\0';
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
		GetTextExtentPointW(myDisplayContext, (const WCHAR*)&ucs2Str.front(), utf8len, &size);
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

void ZLWin32PaintContext::drawString(int x, int y, const char *str, int len) {
	if (myDisplayContext == 0) {
		return;
	}
	adjustPoint(x, y);
	y -= stringHeight();
	y += myTextMetric.tmDescent;
	int utf8len = ZLUnicodeUtil::utf8Length(str, len);
	if (utf8len == len) {
		TextOutA(myDisplayContext, x, y, str, len);
	} else {
		static ZLUnicodeUtil::Ucs2String ucs2Str;
		ucs2Str.clear();
		ZLUnicodeUtil::utf8ToUcs2(ucs2Str, str, len, utf8len);
		TextOutW(myDisplayContext, x, y, (const WCHAR*)&ucs2Str.front(), utf8len);
	}
}

void ZLWin32PaintContext::drawImage(int x, int y, const ZLImageData &image) {
	ZLWin32ImageData &win32Image = (ZLWin32ImageData&)image;
	const BYTE *pixels = win32Image.pixels();
	if (pixels != 0) {
		const int width = image.width();
		const int height = image.height();
		StretchDIBits(myDisplayContext,
			x, y - height, width, height,
			0, 0, width, height,
			pixels, &win32Image.info(), DIB_RGB_COLORS, SRCCOPY);
	}
}

void ZLWin32PaintContext::adjustPoint(int &x, int &y) const {
	x += leftMargin();
	y += topMargin();
}

void ZLWin32PaintContext::drawLine(int x0, int y0, int x1, int y1) {
	if (myDisplayContext == 0) {
		return;
	}

	adjustPoint(x0, y0);
	adjustPoint(x1, y1);

	MoveToEx(myDisplayContext, x0, y0, 0);
	LineTo(myDisplayContext, x1, y1);
	SetPixel(myDisplayContext, x0, y0, myColor);
	SetPixel(myDisplayContext, x1, y1, myColor);
}

void ZLWin32PaintContext::fillRectangle(int x0, int y0, int x1, int y1) {
	if (myDisplayContext == 0) {
		return;
	}

	adjustPoint(x0, y0);
	adjustPoint(x1, y1);

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

	adjustPoint(x, y);

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
		myBackgroundColor = RGB(color.Red, color.Green, color.Blue);
		myBackgroundBrush = CreateSolidBrush(myBackgroundColor);
	}
	RECT rectangle;
	rectangle.top = 0;
	rectangle.bottom = myHeight - 1;
	rectangle.left = 0;
	rectangle.right = myWidth - 1;
	FillRect(myDisplayContext, &rectangle, myBackgroundBrush);
	SetBkMode(myDisplayContext, TRANSPARENT);
}

int ZLWin32PaintContext::width() const {
	return myWidth - leftMargin() - rightMargin();
}

int ZLWin32PaintContext::height() const {
	return myHeight - topMargin() - bottomMargin();
}

HDC ZLWin32PaintContext::displayContext() const {
	return myDisplayContext;
}
