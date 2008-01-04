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

#include <qpainter.h>
#include <qpixmap.h>
#include <qfontmetrics.h>
#include <qfontdatabase.h>
#include <qimage.h>

#include <ZLImage.h>

#include "ZLQtPaintContext.h"
#include "../../qt/image/ZLQtImageManager.h"

ZLQtPaintContext::ZLQtPaintContext() {
	myPainter = new QPainter();
	myPixmap = 0;
	mySpaceWidth = -1;
	myDescent = 0;
	myFontIsStored = false;
}

ZLQtPaintContext::~ZLQtPaintContext() {
	if (myPixmap != 0) {
		myPainter->end();
		delete myPixmap;
	}
	delete myPainter;
}

void ZLQtPaintContext::setSize(int w, int h) {
	if (myPixmap != 0) {
		if ((myPixmap->width() != w) || (myPixmap->height() != h)) {
			myPainter->end();
			delete myPixmap;
			myPixmap = 0;
		}
	}
	if ((myPixmap == 0) && (w > 0) && (h > 0)) {
		myPixmap = new QPixmap(w, h);
		myPainter->begin(myPixmap);
		if (myFontIsStored) {
			myFontIsStored = false;
			setFont(myStoredFamily, myStoredSize, myStoredBold, myStoredItalic);
		}
	}
}

static const std::string HELVETICA = "Helvetica";

void ZLQtPaintContext::fillFamiliesList(std::vector<std::string> &families) const {
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
}

const std::string ZLQtPaintContext::realFontFamilyName(std::string &fontFamily) const {
	QString fullName = QFontInfo(QFont(fontFamily.c_str())).family();
	if (fullName.isNull() || fullName.isEmpty()) {
		fullName = QFontInfo(QFont::defaultFont()).family();
		if (fullName.isNull() || fullName.isEmpty()) {
			return HELVETICA;
		}
	}
	return fullName.left(fullName.find(" [")).ascii();
}

void ZLQtPaintContext::setFont(const std::string &family, int size, bool bold, bool italic) {
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
			myDescent = myPainter->fontMetrics().descent();
		}
	}
}

void ZLQtPaintContext::setColor(ZLColor color, LineStyle style) {
	myPainter->setPen(QPen(
		QColor(color.Red, color.Green, color.Blue),
		1,
		(style == SOLID_LINE) ? QPainter::SolidLine : QPainter::DashLine
	));
}

void ZLQtPaintContext::setFillColor(ZLColor color, FillStyle style) {
	myPainter->setBrush(QBrush(
		QColor(color.Red, color.Green, color.Blue),
		(style == SOLID_FILL) ? QPainter::SolidPattern : QPainter::Dense4Pattern
	));
}

int ZLQtPaintContext::stringWidth(const char *str, int len) const {
	return myPainter->fontMetrics().width(QString::fromUtf8(str, len));
}

int ZLQtPaintContext::spaceWidth() const {
	if (mySpaceWidth == -1) {
		mySpaceWidth = myPainter->fontMetrics().width(" ");
	}
	return mySpaceWidth;
}

int ZLQtPaintContext::descent() const {
	return myDescent;
}

int ZLQtPaintContext::stringHeight() const {
	return myPainter->font().pointSize() + 2;
}

void ZLQtPaintContext::drawString(int x, int y, const char *str, int len) {
	QString qStr = QString::fromUtf8(str, len);
	myPainter->drawText(x, y, qStr);
}

void ZLQtPaintContext::drawImage(int x, int y, const ZLImageData &image) {
	const QImage &qImage = (ZLQtImageData&)image;
	myPainter->drawImage(x, y - qImage.height(), qImage);
}

void ZLQtPaintContext::drawLine(int x0, int y0, int x1, int y1) {
	myPainter->drawLine(x0, y0, x1, y1);
}

void ZLQtPaintContext::fillRectangle(int x0, int y0, int x1, int y1) {
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
	myPainter->fillRect(x0, y0,
											x1 - x0 + 1, y1 - y0 + 1,
											myPainter->brush());
}

void ZLQtPaintContext::drawFilledCircle(int x, int y, int r) {
	myPainter->drawEllipse(x - r, y - r, 2 * r + 1, 2 * r + 1);
}

void ZLQtPaintContext::clear(ZLColor color) {
	if (myPixmap != 0) {
		myPixmap->fill(QColor(color.Red, color.Green, color.Blue));
	}
}

int ZLQtPaintContext::width() const {
	if (myPixmap == 0) {
		return 0;
	}
	return myPixmap->width();
}

int ZLQtPaintContext::height() const {
	if (myPixmap == 0) {
		return 0;
	}
	return myPixmap->height();
}
