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

#include "ZLMirroredPaintContext.h"

ZLMirroredPaintContext::ZLMirroredPaintContext(ZLPaintContext &base) : myBase(base) {
}

void ZLMirroredPaintContext::clear(ZLColor color) {
	myBase.clear(color);
}

void ZLMirroredPaintContext::setFont(const std::string &family, int size, bool bold, bool italic) {
	myBase.setFont(family, size, bold, italic);
}

void ZLMirroredPaintContext::setColor(ZLColor color, LineStyle style) {
	myBase.setColor(color, style);
}

void ZLMirroredPaintContext::setFillColor(ZLColor color, FillStyle style) {
	myBase.setFillColor(color, style);
}

int ZLMirroredPaintContext::width() const {
	return myBase.width();
}

int ZLMirroredPaintContext::height() const {
	return myBase.height();
}

int ZLMirroredPaintContext::stringWidth(const char *str, int len, bool rtl) const {
	return myBase.stringWidth(str, len, rtl);
}

int ZLMirroredPaintContext::spaceWidth() const {
	return myBase.spaceWidth();
}

int ZLMirroredPaintContext::stringHeight() const {
	return myBase.stringHeight();
}

int ZLMirroredPaintContext::descent() const {
	return myBase.descent();
}

void ZLMirroredPaintContext::drawString(int x, int y, const char *str, int len, bool rtl) {
	int w = myBase.stringWidth(str, len, rtl);
	myBase.drawString(mirroredX(x) - w, y, str, len, rtl);
}

void ZLMirroredPaintContext::drawImage(int x, int y, const ZLImageData &image) {
	myBase.drawImage(mirroredX(x) - image.width(), y, image);
}

void ZLMirroredPaintContext::drawImage(int x, int y, const ZLImageData &image, int width, int height, ScalingType type) {
	myBase.drawImage(mirroredX(x) - imageWidth(image, width, height, type), y, image, width, height, type);
}

void ZLMirroredPaintContext::drawLine(int x0, int y0, int x1, int y1) {
	const int w = myBase.width() - 1;
	myBase.drawLine(w - x0, y0, w - x1, y1);
}

void ZLMirroredPaintContext::fillRectangle(int x0, int y0, int x1, int y1) {
	const int w = myBase.width() - 1;
	myBase.fillRectangle(w - x0, y0, w - x1, y1);
}

void ZLMirroredPaintContext::drawFilledCircle(int x, int y, int r) {
	myBase.drawFilledCircle(mirroredX(x), y, r);
}

const std::string ZLMirroredPaintContext::realFontFamilyName(std::string &fontFamily) const {
	return myBase.realFontFamilyName(fontFamily);
}

void ZLMirroredPaintContext::fillFamiliesList(std::vector<std::string> &families) const {
	myBase.fillFamiliesList(families);
}
