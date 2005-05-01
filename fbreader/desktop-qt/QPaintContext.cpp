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

#include <qpainter.h>
#include <qpixmap.h>
#include <qfontmetrics.h>
#include <qfontdatabase.h>
#include <qimage.h>

#include "QPaintContext.h"

#include "../common/model/Image.h"

QPaintContext::QPaintContext() {
	myPainter = new QPainter();
	myPixmap = NULL;
}

QPaintContext::~QPaintContext() {
	if (myPixmap != NULL) {
		myPainter->end();
		delete myPixmap;
	}
	delete myPainter;
}

void QPaintContext::removeCaches() {
	PaintContext::removeCaches();

	for (std::map<const Image*,QImage*>::iterator it = myImageCache.begin(); it != myImageCache.end(); it++) {
		delete it->second;
	}
	myImageCache.clear();
}

void QPaintContext::setSize(int w, int h) {
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
}

void QPaintContext::fillFamiliesList(std::vector<std::string> &families) const {
	QFontDatabase db;
	QStringList qFamilies = db.families();
	for (QStringList::Iterator it = qFamilies.begin(); it != qFamilies.end(); it++) {
		families.push_back((*it).ascii());
	}
}

const std::string QPaintContext::realFontFamilyName(std::string &fontFamily) const {
	QFont font;
	font.setFamily(fontFamily.c_str());
	return font.family().ascii();
}

void QPaintContext::setFont(const std::string &family, int size, bool bold, bool italic) {
	QFont font = myPainter->font();
	font.setFamily(family.c_str());
	font.setPointSize(size);
	font.setWeight(bold ? QFont::Bold : QFont::Normal);
	font.setItalic(italic);
	myPainter->setFont(font);
}

void QPaintContext::setColor(ZLColor color) {
	myPainter->setPen(QColor(color.Red, color.Green, color.Blue));
}

void QPaintContext::setFillColor(ZLColor color) {
	myPainter->setBrush(QColor(color.Red, color.Green, color.Blue));
}

int QPaintContext::wordWidth(const Word &word, int start, int length, bool addHyphenationSign) const {
	QString value = QString::fromUtf8(word.utf8String().data(), word.utf8String().length());
	if ((start == 0) && (length == -1)) {
		return myPainter->fontMetrics().width(value);
	}
	QString subword = value.mid(start, length);
	if (addHyphenationSign) {
		subword.append("-");
	}
	return myPainter->fontMetrics().width(subword);
}

int QPaintContext::spaceWidth() const {
	return myPainter->fontMetrics().width(" ");
}

int QPaintContext::wordHeight() const {
	return myPainter->font().pointSize() + 2;
}

void QPaintContext::drawQString(int x, int y, const QString &str, const Word::WordMark *mark, int shift) {
	if (mark == 0) {
		myPainter->drawText(x, y, str);
	} else {
		QPen textPen = myPainter->pen();
		int currentLetter = 0;
		for (; (mark != 0) && (currentLetter < (int)str.length()); mark = mark->next()) {
			int markStart = mark->start() - shift;
			int markLen = mark->length();

			if (markStart < currentLetter) {
				markLen += markStart - currentLetter;
				markStart = currentLetter;
			}

			if (markLen <= 0) {
				continue;
			}

			if (markStart > currentLetter) {
				QString beforeMark = str.mid(currentLetter, markStart - currentLetter);
				myPainter->drawText(x, y, beforeMark);
				x += myPainter->fontMetrics().width(beforeMark);
			}
			QString insideMark = str.mid(markStart, markLen);
			setColor(mySelectedTextColor);
			myPainter->drawText(x, y, insideMark);
			myPainter->setPen(textPen);
			x += myPainter->fontMetrics().width(insideMark);
			currentLetter = markStart + markLen;
		}

		if (currentLetter < (int)str.length()) {
			QString afterMark = str.mid(currentLetter);
			myPainter->drawText(x, y, afterMark);
		}
	}
}

void QPaintContext::drawWord(int x, int y, const Word &word, int start, int length, bool addHyphenationSign) {
	QString value = QString::fromUtf8(word.utf8String().data(), word.utf8String().length());
	x += leftMargin().value();
	y += topMargin().value();
	if ((start == 0) && (length == -1)) {
		drawQString(x, y, value, word.mark());
	} else {
		QString subword = value.mid(start, length);
		if (addHyphenationSign) {
			subword.append("-");
		}
		drawQString(x, y, subword, word.mark(), start);
	}
}

QImage &QPaintContext::qImage(const Image &image) const {
	QImage *imageRef = myImageCache[&image];
	if (imageRef == NULL) {
		imageRef = new QImage();
		imageRef->loadFromData(image.data(), image.datalen());
		myImageCache[&image] = imageRef;
	}
	return *imageRef;
}

int QPaintContext::imageWidth(const Image &image) const {
	int w = qImage(image).width();
	int maxW = width();
	return (w <= maxW) ? w : maxW;
}

int QPaintContext::imageHeight(const Image &image) const {
	return qImage(image).height();
}

void QPaintContext::drawImage(int x, int y, const Image &image) {
	myPainter->drawImage(x + leftMargin().value(), y + topMargin().value() - imageHeight(image), qImage(image));
}

void QPaintContext::drawLine(int x0, int y0, int x1, int y1) {
	myPainter->drawLine(x0 + leftMargin().value(), y0 + topMargin().value(),
											x1 + leftMargin().value(), y1 + topMargin().value());
}

void QPaintContext::fillRectangle(int x0, int y0, int x1, int y1) {
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
	myPainter->fillRect(x0 + leftMargin().value(), y0 + topMargin().value(),
											x1 - x0 + 1, y1 - y0 + 1,
											myPainter->brush());
}

void QPaintContext::clear() {
	if (myPixmap != NULL) {
		ZLColor background = BackgroundColorOption.value();
		myPixmap->fill(QColor(background.Red, background.Green, background.Blue));
		mySelectedTextColor = SelectedTextColorOption.value();
	}
}

int QPaintContext::width() const {
	if (myPixmap == NULL) {
		return 0;
	}
	return myPixmap->width() - leftMargin().value() - rightMargin().value();
}

int QPaintContext::height() const {
	if (myPixmap == NULL) {
		return 0;
	}
	return myPixmap->height() - bottomMargin().value() - topMargin().value();
}
