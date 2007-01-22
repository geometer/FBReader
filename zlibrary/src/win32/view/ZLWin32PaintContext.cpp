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

ZLWin32PaintContext::ZLWin32PaintContext() : myLineStyle((LineStyle)-1), myBackgroundBrush(0), myFillBrush(0) {
	/*
	myPainter = new QPainter();
	myPixmap = NULL;
	mySpaceWidth = -1;
	myFontIsStored = false;
	*/
}

ZLWin32PaintContext::~ZLWin32PaintContext() {
	if (myBackgroundBrush != 0) {
		DeleteObject(myBackgroundBrush);
	}
	if (myFillBrush != 0) {
		DeleteObject(myFillBrush);
	}
	/*
	if (myPixmap != NULL) {
		myPainter->end();
		delete myPixmap;
	}
	delete myPainter;
	*/
}

//#include <stdlib.h>

void ZLWin32PaintContext::beginPaint(ZLWin32ApplicationWindow &window) {
	if (myWindow == 0) {
		myWindow = window.mainWindow();
		myTopOffset = window.topOffset();
		myDisplayContext = BeginPaint(myWindow, &myPaintStructure);
		//_sleep(5000);
	}
}

void ZLWin32PaintContext::endPaint() {
	if (myWindow != 0) {
		EndPaint(myWindow, &myPaintStructure);
		myWindow = 0;
	}
}

void ZLWin32PaintContext::setSize(int w, int h) {
	/*
	if (myPixmap != NULL) {
		if ((myPixmap->width() != w) || (myPixmap->height() != h)) {
			myPainter->end();
			delete myPixmap;
			myPixmap = NULL;
		}
	}
	if ((myPixmap == NULL) && (w > 0) && (h > 0)) {
		myPixmap = new QPixmap(w, h);
		myPainter->begin(myPixmap);
		if (myFontIsStored) {
			myFontIsStored = false;
			setFont(myStoredFamily, myStoredSize, myStoredBold, myStoredItalic);
		}
	}
	*/
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
	if (myWindow == 0) {
		return;
	}
	// TODO: optimize
	LOGFONT logicalFont;
	memset(&logicalFont, 0, sizeof(LOGFONT));
	logicalFont.lfHeight = size;
	logicalFont.lfWeight = bold ? 700 : 400;
	logicalFont.lfItalic = italic;
	const int len = std::min((int)family.size(), LF_FACESIZE - 1);
	strncpy(logicalFont.lfFaceName, family.data(), len);
	logicalFont.lfFaceName[len] = '\0';
	HFONT font = CreateFontIndirect(&logicalFont);
	DeleteObject(SelectObject(myDisplayContext, font));
}

void ZLWin32PaintContext::setColor(ZLColor color, LineStyle style) {
	if (myWindow == 0) {
		return;
	}
	if ((color != myColor) || (style != myLineStyle)) {
		myColor = color;
		myLineStyle = style;
		myColorref = RGB(color.Red, color.Green, color.Blue);
		DeleteObject(SelectObject(myDisplayContext, CreatePen((style == ZLPaintContext::SOLID_LINE) ? PS_SOLID : PS_DASH, 1, myColorref)));
	}
}

void ZLWin32PaintContext::setFillColor(ZLColor color, FillStyle style) {
	if (myWindow == 0) {
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
	/*
	return myPainter->fontMetrics().width(QString::fromUtf8(str, len));
	*/
	return 20 * len;
}

int ZLWin32PaintContext::spaceWidth() const {
	/*
	if (mySpaceWidth == -1) {
		mySpaceWidth = myPainter->fontMetrics().width(" ");
	}
	return mySpaceWidth;
	*/
	return 20;
}

int ZLWin32PaintContext::stringHeight() const {
	/*
	return myPainter->font().pointSize() + 2;
	*/
	return 20;
}

void ZLWin32PaintContext::drawString(int x, int y, const char *str, int len) {
	if (myWindow == 0) {
		return;
	}
	adjustPoint(x, y);
	int utf8len = ZLUnicodeUtil::utf8Length(str, len);
	if (utf8len == len) {
		TextOut(myDisplayContext, x, y, str, len);
	} else {
		// TODO: optimize (?)
		ZLUnicodeUtil::Ucs2String ucs2Str(utf8len);
		ZLUnicodeUtil::utf8ToUcs2(ucs2Str, str, len, utf8len);
		TextOutW(myDisplayContext, x, y, (const WCHAR*)&ucs2Str.front(), utf8len);
	}
}

void ZLWin32PaintContext::drawImage(int x, int y, const ZLImageData &image) {
	/*
	const QImage &qImage = (ZLWin32ImageData&)image;
	myPainter->drawImage(x + leftMargin(), y + topMargin() - qImage.height(), qImage);
	*/
}

void ZLWin32PaintContext::adjustPoint(int &x, int &y) const {
	x += leftMargin();
	y += myTopOffset + topMargin();
}

void ZLWin32PaintContext::drawLine(int x0, int y0, int x1, int y1) {
	if (myWindow == 0) {
		return;
	}

	adjustPoint(x0, y0);
	adjustPoint(x1, y1);

	MoveToEx(myDisplayContext, x0, y0, 0);
	LineTo(myDisplayContext, x1, y1);
	SetPixel(myDisplayContext, x0, y0, myColorref);
	SetPixel(myDisplayContext, x1, y1, myColorref);
}

void ZLWin32PaintContext::fillRectangle(int x0, int y0, int x1, int y1) {
	if (myWindow == 0) {
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
	if (myWindow == 0) {
		return;
	}

	adjustPoint(x, y);

	HBRUSH oldBrush = (HBRUSH)SelectObject(myDisplayContext, myFillBrush);
	Ellipse(myDisplayContext, x - r, y - r, x + r, y + r);
	SelectObject(myDisplayContext, oldBrush);
}

void ZLWin32PaintContext::clear(ZLColor color) {
	if (myWindow == 0) {
		return;
	}
	if ((myBackgroundBrush == 0) || (color != myBackgroundColor)) {
		if (myBackgroundBrush != 0) {
			DeleteObject(myBackgroundBrush);
		}
		myBackgroundColor = color;
		myBackgroundBrush = CreateSolidBrush(RGB(color.Red, color.Green, color.Blue));
		// TODO: set background brush
		//SetWindowLong(myWindow, GCL_HBRBACKGROUND, (LONG)myBackgroundBrush);
	}
	RECT rectangle;
	GetClientRect(myWindow, &rectangle);
	rectangle.top += myTopOffset;
	FillRect(myDisplayContext, &rectangle, myBackgroundBrush);
	SetBkColor(myDisplayContext, RGB(color.Red, color.Green, color.Blue));
}

int ZLWin32PaintContext::width() const {
	/*
	if (myPixmap == NULL) {
		return 0;
	}
	return myPixmap->width() - leftMargin() - rightMargin();
	*/
	return 200;
}

int ZLWin32PaintContext::height() const {
	/*
	if (myPixmap == NULL) {
		return 0;
	}
	return myPixmap->height() - bottomMargin() - topMargin();
	*/
	return 200;
}
