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

#include "ZLWin32PaintContext.h"

ZLWin32PaintContext::ZLWin32PaintContext() : myCurrentLineStyle((LineStyle)-1), myBackgroundBrush(0), myFillBrush(0) {
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

void ZLWin32PaintContext::beginPaint(HWND window) {
	myWindow = window;
	// TODO: don't clear background before painting
	myDisplayContext = BeginPaint(myWindow, &myPaintStructure);
	//_sleep(5000);
}

void ZLWin32PaintContext::endPaint() {
	EndPaint(myWindow, &myPaintStructure);
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
	/*
	if (myPainter->device() == 0) {
		myFontIsStored = true;
		myStoredFamily = family;
		myStoredSize = size;
		myStoredBold = bold;
		myStoredItalic= italic;
	} else {
		QFont font = myPainter->font();
		bool fontChanged = false;

		if (font.family() != family.c_str()) {
			font.setFamily(family.c_str());
			fontChanged = true;
		}

		if (font.pointSize() != size) {
			font.setPointSize(size);
			fontChanged = true;
		}

		if ((font.weight() != (bold ? QFont::Bold : QFont::Normal))) {
			font.setWeight(bold ? QFont::Bold : QFont::Normal);
			fontChanged = true;
		}

		if (font.italic() != italic) {
			font.setItalic(italic);
			fontChanged = true;
		}

		if (fontChanged) {
			myPainter->setFont(font);
			mySpaceWidth = -1;
		}
	}
	*/
}

void ZLWin32PaintContext::setColor(ZLColor color, LineStyle style) {
	if ((color != myCurrentColor) || (style != myCurrentLineStyle)) {
		myCurrentColor = color;
		myCurrentLineStyle = style;
		myCurrentColorref = RGB(color.Red, color.Green, color.Blue);
		DeleteObject(SelectObject(myDisplayContext, CreatePen((style == ZLPaintContext::SOLID_LINE) ? PS_SOLID : PS_DASH, 1, myCurrentColorref)));
	}
}

void ZLWin32PaintContext::setFillColor(ZLColor color, FillStyle style) {
	// TODO: use style
	if (myFillBrush != 0) {
		DeleteObject(myFillBrush);
	}
	COLORREF colorref = RGB(color.Red, color.Green, color.Blue);
	myFillBrush =
		(style == SOLID_FILL) ?
			CreateSolidBrush(colorref) :
			CreateHatchBrush(HS_DIAGCROSS, colorref);
	/*
	myPainter->setBrush(QBrush(
		QColor(color.Red, color.Green, color.Blue),
		(style == SOLID_FILL) ? QPainter::SolidPattern : QPainter::Dense4Pattern
	));
	*/
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
	/*
	QString qStr = QString::fromUtf8(str, len);
	myPainter->drawText(x + leftMargin(), y + topMargin(), qStr);
	*/
}

void ZLWin32PaintContext::drawImage(int x, int y, const ZLImageData &image) {
	/*
	const QImage &qImage = (ZLWin32ImageData&)image;
	myPainter->drawImage(x + leftMargin(), y + topMargin() - qImage.height(), qImage);
	*/
}

void ZLWin32PaintContext::drawLine(int x0, int y0, int x1, int y1) {
	x0 += leftMargin();
	x1 += leftMargin();
	y0 += topMargin();
	y1 += topMargin();
	/*
	myPainter->drawPoint(x0, y0);
	myPainter->drawLine(x0, y0, x1, y1);
	myPainter->drawPoint(x1, y1);
	*/
	MoveToEx(myDisplayContext, x0, y0, 0);
	LineTo(myDisplayContext, x1, y1);
	SetPixel(myDisplayContext, x0, y0, myCurrentColorref);
	SetPixel(myDisplayContext, x1, y1, myCurrentColorref);
}

void ZLWin32PaintContext::fillRectangle(int x0, int y0, int x1, int y1) {
	if (x1 < x0) {
		int tmp = x1;
		x1 = x0;
		x0 = tmp;
	}
	if (y1 < y0) {
		int tmp = y1;
		y1 = y0;
		y0 = tmp;
	}
	RECT rectangle;
	rectangle.left = x0;
	rectangle.right = x1 + 1;
	rectangle.top = y0;
	rectangle.bottom = y1 + 1;
	FillRect(myDisplayContext, &rectangle, myFillBrush);
	/*
	myPainter->fillRect(x0 + leftMargin(), y0 + topMargin(),
											x1 - x0 + 1, y1 - y0 + 1,
											myPainter->brush());
	*/
}

void ZLWin32PaintContext::drawFilledCircle(int x, int y, int r) {
	x += leftMargin();
	y += topMargin();
	HBRUSH oldBrush = (HBRUSH)SelectObject(myDisplayContext, myFillBrush);
	Ellipse(myDisplayContext, x - r, y - r, x + r, y + r);
	SelectObject(myDisplayContext, oldBrush);
}

void ZLWin32PaintContext::clear(ZLColor color) {
	// TODO: don't change brush if color equals oldcolor
	// TODO: set default background color instead of FillRect
	if (myBackgroundBrush != 0) {
		DeleteObject(myBackgroundBrush);
	}
	myBackgroundBrush = CreateSolidBrush(RGB(color.Red, color.Green, color.Blue));
	RECT rectangle;
	GetClientRect(myWindow, &rectangle);
	FillRect(myDisplayContext, &rectangle, myBackgroundBrush);
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
