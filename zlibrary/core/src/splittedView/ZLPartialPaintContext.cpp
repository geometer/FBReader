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

#include "ZLPartialPaintContext.h"
#include "ZLSplittedView.h"

ZLPartialPaintContext::ZLPartialPaintContext(ZLSplittedView &splittedView, bool left) : mySplittedView(splittedView), myIsLeft(left) {
}

void ZLPartialPaintContext::clear(ZLColor color) {
	mySplittedView.context().setFillColor(color);
	int delta = xDelta();
	mySplittedView.context().fillRectangle(delta, 0, mySplittedView.partWidth(myIsLeft) + delta, mySplittedView.context().height());
}

void ZLPartialPaintContext::setFont(const std::string &family, int size, bool bold, bool italic) {
	mySplittedView.context().setFont(family, size, bold, italic);
}

void ZLPartialPaintContext::setColor(ZLColor color, LineStyle style) {
	mySplittedView.context().setColor(color, style);
}

void ZLPartialPaintContext::setFillColor(ZLColor color, FillStyle style) {
	mySplittedView.context().setFillColor(color, style);
}

int ZLPartialPaintContext::width() const {
	return mySplittedView.partWidth(myIsLeft);
}

int ZLPartialPaintContext::height() const {
	return mySplittedView.context().height();
}

int ZLPartialPaintContext::stringWidth(const char *str, int len) const {
	return mySplittedView.context().stringWidth(str, len);
}

int ZLPartialPaintContext::spaceWidth() const {
	return mySplittedView.context().spaceWidth();
}

int ZLPartialPaintContext::stringHeight() const {
	return mySplittedView.context().stringHeight();
}

int ZLPartialPaintContext::descent() const {
	return mySplittedView.context().descent();
}

void ZLPartialPaintContext::drawString(int x, int y, const char *str, int len) {
	mySplittedView.context().drawString(x + xDelta(), y, str, len);
}

void ZLPartialPaintContext::drawImage(int x, int y, const ZLImageData &image) {
	mySplittedView.context().drawImage(x + xDelta(), y, image);
}

void ZLPartialPaintContext::drawLine(int x0, int y0, int x1, int y1) {
	int delta = xDelta();
	mySplittedView.context().drawLine(x0 + delta, y0, x1 + delta, y1);
}

void ZLPartialPaintContext::fillRectangle(int x0, int y0, int x1, int y1) {
	int delta = xDelta();
	mySplittedView.context().fillRectangle(x0 + delta, y0, x1 + delta, y1);
}

void ZLPartialPaintContext::drawFilledCircle(int x, int y, int r) {
	mySplittedView.context().drawFilledCircle(x + xDelta(), y, r);
}

const std::string ZLPartialPaintContext::realFontFamilyName(std::string &fontFamily) const {
	return mySplittedView.context().realFontFamilyName(fontFamily);
}

void ZLPartialPaintContext::fillFamiliesList(std::vector<std::string> &families) const {
	mySplittedView.context().fillFamiliesList(families);
}

int ZLPartialPaintContext::xDelta() const {
	if (myIsLeft) {
		return 0;
	} else {
		return mySplittedView.partWidth(true) + mySplittedView.splitterWidth() + 1;
	}
}
