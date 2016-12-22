/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QFontMetrics>
#include <QtGui/QFontDatabase>
#include <QtGui/QImage>

#include <ZLImage.h>

#include "ZLQmlPaintContext.h"
#include "../image/ZLQtImageManager.h"

ZLQmlPaintContext::ZLQmlPaintContext() {
	myPainter = 0;
	myWidth = 0;
	myHeight = 0;
	mySpaceWidth = -1;
	myDescent = 0;
}

ZLQmlPaintContext::~ZLQmlPaintContext() {
}

void ZLQmlPaintContext::beginPaint(int w, int h, QPainter *painter) {
	myPainter = painter;
	myWidth = w;
	myHeight = h;
	myPainter->setFont(myFont);
}

void ZLQmlPaintContext::endPaint() {
	myPainter = 0;
}

static const std::string HELVETICA = "Helvetica";

void ZLQmlPaintContext::fillFamiliesList(std::vector<std::string> &families) const {
	QFontDatabase db;
	const QStringList qFamilies = db.families();
	bool helveticaFlag = false;
	for (QStringList::ConstIterator it = qFamilies.begin(); it != qFamilies.end(); ++it) {
		std::string family = it->toUtf8().constData();
		helveticaFlag |= (family == HELVETICA);
		families.push_back(family);
	}
	if (!helveticaFlag) {
		families.push_back(HELVETICA);
	}
}

const std::string ZLQmlPaintContext::realFontFamilyName(std::string &fontFamily) const {
	QString fullName = QFontInfo(QFont(QString::fromUtf8(fontFamily.c_str()))).family();
	if (fullName.isNull() || fullName.isEmpty()) {
		return HELVETICA;
	}
	//return fullName.left(fullName.find(" [")).ascii();
	return (const char*)fullName.toUtf8();
}

void ZLQmlPaintContext::setFont(const std::string &family, int size, bool bold, bool italic) {
	bool fontChanged = false;

	if (myFont.family() != QLatin1String(family.c_str())) {
		myFont.setFamily(QLatin1String(family.c_str()));
		fontChanged = true;
	}

	if (myFont.pointSize() != size) {
		myFont.setPointSize(size);
		fontChanged = true;
	}

	if ((myFont.weight() != (bold ? QFont::Bold : QFont::Normal))) {
		myFont.setWeight(bold ? QFont::Bold : QFont::Normal);
		fontChanged = true;
	}

	if (myFont.italic() != italic) {
		myFont.setItalic(italic);
		fontChanged = true;
	}

	if (fontChanged) {
		mySpaceWidth = -1;
		QFontMetrics fontMetrics(myFont);
		myDescent = fontMetrics.descent();
		if (myPainter)
			myPainter->setFont(myFont);
	}
}

void ZLQmlPaintContext::setColor(ZLColor color, LineStyle style) {
	if (!myPainter)
		return;
	myPainter->setPen(QPen(
		QColor(color.Red, color.Green, color.Blue),
		1,
		(style == SOLID_LINE) ? Qt::SolidLine : Qt::DashLine
	));
}

void ZLQmlPaintContext::setFillColor(ZLColor color, FillStyle style) {
	if (!myPainter)
		return;
	myPainter->setBrush(QBrush(
		QColor(color.Red, color.Green, color.Blue),
		(style == SOLID_FILL) ? Qt::SolidPattern : Qt::Dense4Pattern
	));
}

int ZLQmlPaintContext::stringWidth(const char *str, int len, bool) const {
	QFontMetrics fontMetrics(myFont);
	return fontMetrics.width(QString::fromUtf8(str, len));
}

int ZLQmlPaintContext::spaceWidth() const {
	if (mySpaceWidth == -1 && myPainter) {
		mySpaceWidth = myPainter->fontMetrics().width(' ');
	}
	return mySpaceWidth;
}

int ZLQmlPaintContext::descent() const {
	return myDescent;
}

int ZLQmlPaintContext::stringHeight() const {
	return myFont.pointSize() + 2;
}

void ZLQmlPaintContext::drawString(int x, int y, const char *str, int len, bool rtl) {
	if (!myPainter)
		return;
	QString qStr = QString::fromUtf8(str, len);
	myPainter->setLayoutDirection(rtl ? Qt::RightToLeft : Qt::LeftToRight);
	myPainter->drawText(x, y, qStr);
}

void ZLQmlPaintContext::drawImage(int x, int y, const ZLImageData &image) {
	if (!myPainter)
		return;
	const QImage *qImage = ((ZLQtImageData&)image).image();
	if (qImage != 0) {
		myPainter->drawImage(x, y - image.height(), *qImage);
	}
}

void ZLQmlPaintContext::drawImage(int x, int y, const ZLImageData &image, int width, int height, ScalingType type) {
	if (!myPainter)
		return;
	const QImage *qImage = ((ZLQtImageData&)image).image();
	if (qImage == 0) {
		return;
	}
	const QImage scaled = qImage->scaled(
		QSize(imageWidth(image, width, height, type),
					imageHeight(image, width, height, type)),
		Qt::KeepAspectRatio,
		Qt::SmoothTransformation
	);
	myPainter->drawImage(x, y - scaled.height(), scaled);
}

void ZLQmlPaintContext::drawLine(int x0, int y0, int x1, int y1) {
	if (!myPainter)
		return;
	myPainter->drawPoint(x0, y0);
	myPainter->drawLine(x0, y0, x1, y1);
	myPainter->drawPoint(x1, y1);
}

void ZLQmlPaintContext::fillRectangle(int x0, int y0, int x1, int y1) {
	if (!myPainter)
		return;
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

void ZLQmlPaintContext::drawFilledCircle(int x, int y, int r) {
	if (!myPainter)
		return;
	myPainter->drawEllipse(x - r, y - r, 2 * r + 1, 2 * r + 1);
}

void ZLQmlPaintContext::clear(ZLColor color) {
	if (!myPainter)
		return;
	myPainter->fillRect(0, 0, myWidth, myHeight,
	                    QColor(color.Red, color.Green, color.Blue));
}

int ZLQmlPaintContext::width() const {
	return myWidth;
}

int ZLQmlPaintContext::height() const {
	return myHeight;
}
