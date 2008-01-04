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

#include <ZLImage.h>

#include "ZLQtImageManager.h"

void ZLQtImageData::init(unsigned int width, unsigned int height) {
	create(width, height, 32);
}

void ZLQtImageData::setPosition(unsigned int x, unsigned int y) {
	myX = x;
	myY = y;
}

void ZLQtImageData::moveX(int delta) {
	myX += delta;
}

void ZLQtImageData::moveY(int delta) {
	myY += delta;
}

void ZLQtImageData::setPixel(unsigned char r, unsigned char g, unsigned char b) {
	QImage::setPixel(myX, myY, qRgb(r, g, b));
}

void ZLQtImageData::copyFrom(const ZLImageData &source, unsigned int targetX, unsigned int targetY) {
	bitBlt(this, targetX, targetY, (const ZLQtImageData*)&source, 0, 0);
}

shared_ptr<ZLImageData> ZLQtImageManager::createData() const {
	return new ZLQtImageData();
}

void ZLQtImageManager::convertImageDirect(const std::string &stringData, ZLImageData &data) const {
	((ZLQtImageData&)data).loadFromData((const unsigned char*)stringData.data(), stringData.length());
}
