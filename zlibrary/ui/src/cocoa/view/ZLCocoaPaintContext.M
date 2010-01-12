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

#include <iostream>
//#include <algorithm>

//#include <ZLUnicodeUtil.h>
//#include <ZLImage.h>

#include "ZLCocoaPaintContext.h"
//#include "../image/ZLCocoaImageManager.h"

ZLCocoaPaintContext::ZLCocoaPaintContext() {
}

ZLCocoaPaintContext::~ZLCocoaPaintContext() {
}

void ZLCocoaPaintContext::fillFamiliesList(std::vector<std::string> &families) const {
	// TODO: implement
}

const std::string ZLCocoaPaintContext::realFontFamilyName(std::string &fontFamily) const {
	// TODO: implement
	return fontFamily;
}

void ZLCocoaPaintContext::setFont(const std::string &family, int size, bool bold, bool italic) {
	// TODO: implement
}

void ZLCocoaPaintContext::setColor(ZLColor color, LineStyle style) {
	// TODO: implement
}

void ZLCocoaPaintContext::setFillColor(ZLColor color, FillStyle style) {
	// TODO: implement
}

int ZLCocoaPaintContext::stringWidth(const char *str, int len, bool rtl) const {
	// TODO: implement
	return 10 * len;
}

int ZLCocoaPaintContext::spaceWidth() const {
	// TODO: implement
	return 10;
}

int ZLCocoaPaintContext::stringHeight() const {
	// TODO: implement
	return 10;
}

int ZLCocoaPaintContext::descent() const {
	// TODO: implement
	return 0;
}

void ZLCocoaPaintContext::drawString(int x, int y, const char *str, int len, bool rtl) {
	// TODO: implement
}

void ZLCocoaPaintContext::drawImage(int x, int y, const ZLImageData &image) {
	// TODO: implement
}

void ZLCocoaPaintContext::drawImage(int x, int y, const ZLImageData &image, int width, int height, ScalingType type) {
	// TODO: implement
}

void ZLCocoaPaintContext::drawLine(int x0, int y0, int x1, int y1) {
	// TODO: implement
}

void ZLCocoaPaintContext::fillRectangle(int x0, int y0, int x1, int y1) {
	// TODO: implement
}

void ZLCocoaPaintContext::drawFilledCircle(int x, int y, int r) {
	// TODO: implement
}

void ZLCocoaPaintContext::clear(ZLColor color) {
	std::cerr << "CLEAR\n";
	// TODO: implement
}

int ZLCocoaPaintContext::width() const {
	// TODO: implement
	return 500;
}

int ZLCocoaPaintContext::height() const {
	// TODO: implement
	return 500;
}

// vim:ts=2:sw=2:noet
