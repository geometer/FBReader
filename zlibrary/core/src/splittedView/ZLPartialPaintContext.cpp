/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "ZLPartialPaintContext.h"

ZLPartialPaintContext::ZLPartialPaintContext(ZLPaintContext &base, bool left) : myBaseContext(base), myIsLeft(left) {
}

void ZLPartialPaintContext::clear(ZLColor color) {
	// TODO: implement
}

void ZLPartialPaintContext::setFont(const std::string &family, int size, bool bold, bool italic) {
	myBaseContext.setFont(family, size, bold, italic);
}

void ZLPartialPaintContext::setColor(ZLColor color, LineStyle style) {
	myBaseContext.setColor(color, style);
}

void ZLPartialPaintContext::setFillColor(ZLColor color, FillStyle style) {
	myBaseContext.setFillColor(color, style);
}

int ZLPartialPaintContext::width() const {
	return myBaseContext.width() / 2 - 1;
}

int ZLPartialPaintContext::height() const {
	return myBaseContext.height();
}

int ZLPartialPaintContext::stringWidth(const char *str, int len) const {
	return myBaseContext.stringWidth(str, len);
}

int ZLPartialPaintContext::spaceWidth() const {
	return myBaseContext.spaceWidth();
}

int ZLPartialPaintContext::stringHeight() const {
	return myBaseContext.stringHeight();
}

int ZLPartialPaintContext::descent() const {
	return myBaseContext.descent();
}

void ZLPartialPaintContext::drawString(int x, int y, const char *str, int len) {
	// TODO: implement
}

void ZLPartialPaintContext::drawImage(int x, int y, const ZLImageData &image) {
	// TODO: implement
}

void ZLPartialPaintContext::drawLine(int x0, int y0, int x1, int y1) {
	// TODO: implement
}

void ZLPartialPaintContext::fillRectangle(int x0, int y0, int x1, int y1) {
	// TODO: implement
}

void ZLPartialPaintContext::drawFilledCircle(int x, int y, int r) {
	// TODO: implement
}

const std::string ZLPartialPaintContext::realFontFamilyName(std::string &fontFamily) const {
	return myBaseContext.realFontFamilyName(fontFamily);
}

void ZLPartialPaintContext::fillFamiliesList(std::vector<std::string> &families) const {
	myBaseContext.fillFamiliesList(families);
}
