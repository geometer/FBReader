/*
 * FBReader -- electronic book reader
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

#include <PalmOS.h>				

#include "PalmPaintContext.h"

#include "../common/model/Image.h"

PalmPaintContext::PalmPaintContext() {
	//myPainter = new QPainter();
	//myPixmap = NULL;
	myWidth = 0;
	myHeight = 0;
}

PalmPaintContext::~PalmPaintContext() {
	/*
	if (myPixmap != NULL) {
		myPainter->end();
		delete myPixmap;
	}
	delete myPainter;
	*/
}

void PalmPaintContext::removeCaches() {
	PaintContext::removeCaches();

	/*
	for (std::map<const Image*,QImage*>::iterator it = myImageCache.begin(); it != myImageCache.end(); it++) {
		delete it->second;
	}
	myImageCache.clear();
	*/
}

void PalmPaintContext::setSize(int w, int h) {
	myWidth = w;
	myHeight = h;
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
	}
	*/
}

void PalmPaintContext::fillFamiliesList(std::vector<std::string> &families) const {
	/*
	QFontDatabase db;
	QStringList qFamilies = db.families();
	for (QStringList::Iterator it = qFamilies.begin(); it != qFamilies.end(); it++) {
		families.push_back((*it).ascii());
	}
	*/
}

const std::string PalmPaintContext::realFontFamilyName(std::string &fontFamily) const {
	/*
	QFont font;
	font.setFamily(fontFamily.c_str());
	return font.family().ascii();
	*/
	return fontFamily;
}

void PalmPaintContext::setFont(const std::string &family, int size, bool bold, bool italic) {
	/*
	QFont font = myPainter->font();
	font.setFamily(family.c_str());
	font.setPointSize(size);
	font.setWeight(bold ? QFont::Bold : QFont::Normal);
	font.setItalic(italic);
	myPainter->setFont(font);
	*/
}

static IndexedColorType iColor(ZLColor zlColor) {
	RGBColorType rgbColor = { 0x00, zlColor.Red, zlColor.Green, zlColor.Blue };
	return WinRGBToIndex(&rgbColor);
}

void PalmPaintContext::setColor(ZLColor color) {
	IndexedColorType c = iColor(color);
	WinSetForeColor(c);
	WinSetTextColor(c);
}

void PalmPaintContext::setFillColor(ZLColor color) {
	IndexedColorType c = iColor(color);
	//WinSetBackColor(c);
}

int PalmPaintContext::stringWidth(const std::string &str, int from, int len) const {
	//return myPainter->fontMetrics().width(QString::fromUtf8(str.data() + from, len));
	return 100;
}

int PalmPaintContext::stringHeight() const {
	//return myPainter->font().pointSize() + 2;
	return 20;
}

void PalmPaintContext::drawString(int x, int y, const std::string &str, int from, int len) {
	//QString qStr = QString::fromUtf8(str.data() + from, len);
	//myPainter->drawText(x + leftMargin().value(), y + topMargin().value(), qStr);
	WinDrawChars(str.data() + from, len, x + leftMargin().value(), y + topMargin().value());
}

/*
QImage &PalmPaintContext::qImage(const Image &image) const {
	QImage *imageRef = myImageCache[&image];
	if (imageRef == NULL) {
		imageRef = new QImage();
		imageRef->loadFromData(image.data(), image.datalen());
		myImageCache[&image] = imageRef;
	}
	return *imageRef;
}
*/

int PalmPaintContext::imageWidth(const Image &image) const {
	/*
	int w = qImage(image).width();
	int maxW = width();
	return (w <= maxW) ? w : maxW;
	*/
	return 100;
}

int PalmPaintContext::imageHeight(const Image &image) const {
	//return qImage(image).height();
	return 100;
}

void PalmPaintContext::drawImage(int x, int y, const Image &image) {
	//myPainter->drawImage(x + leftMargin().value(), y + topMargin().value() - imageHeight(image), qImage(image));
}

void PalmPaintContext::drawLine(int x0, int y0, int x1, int y1) {
	WinDrawLine(x0 + leftMargin().value(), y0 + topMargin().value(),
							x1 + leftMargin().value(), y1 + topMargin().value());
}

void PalmPaintContext::fillRectangle(int x0, int y0, int x1, int y1) {
	RectangleType r;
	if (x0 < x1) {
		r.topLeft.x = x0;
		r.extent.x = x1 - x0 + 1;
	} else {
		r.topLeft.x = x1;
		r.extent.x = x0 - x1 + 1;
	}
	if (y0 < y1) {
		r.topLeft.y = y0;
		r.extent.y = y1 - y0 + 1;
	} else {
		r.topLeft.y = y1;
		r.extent.y = y0 - y1 + 1;
	}
	WinFillRectangle(&r, 0);
	/*
	myPainter->fillRect(x0 + leftMargin().value(), y0 + topMargin().value(),
											x1 - x0 + 1, y1 - y0 + 1,
											myPainter->brush());
	*/
}

void PalmPaintContext::clear() {
	WinSetBackColor(iColor(BackgroundColorOption.value()));
	RectangleType r;
	r.topLeft.x = 0;
	r.topLeft.y = 0;
	r.extent.x = myWidth;
	r.extent.y = myHeight;
	WinFillRectangle(&r, 0);
	/*
	if (myPixmap != NULL) {
		ZLColor background = BackgroundColorOption.value();
		myPixmap->fill(QColor(background.Red, background.Green, background.Blue));
	}
	*/
}

int PalmPaintContext::width() const {
	/*
	if (myPixmap == NULL) {
		return 0;
	}
	return myPixmap->width() - leftMargin().value() - rightMargin().value();
	*/
	return myWidth - leftMargin().value() - rightMargin().value();
}

int PalmPaintContext::height() const {
	/*
	if (myPixmap == NULL) {
		return 0;
	}
	return myPixmap->height() - bottomMargin().value() - topMargin().value();
	*/
	return myHeight - bottomMargin().value() - topMargin().value();
}
