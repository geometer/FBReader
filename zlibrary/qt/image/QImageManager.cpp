/*
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <iostream>

#include <abstract/ZLImage.h>

#include "QImageManager.h"

void ZLQImageData::init(unsigned int width, unsigned int height) {
	create(width, height, 32);
}

void ZLQImageData::setPosition(unsigned int x, unsigned int y) {
	myX = x;
	myY = y;
}

void ZLQImageData::moveX(int delta) {
	myX += delta;
}

void ZLQImageData::moveY(int delta) {
	myY += delta;
}

void ZLQImageData::setPixel(unsigned char r, unsigned char g, unsigned char b) {
	QImage::setPixel(myX, myY, qRgb(r, g, b));
}

void ZLQImageData::copyFrom(const ZLImageData &source, unsigned int sourceX, unsigned int sourceY, unsigned int targetX, unsigned int targetY) {
	bitBlt(this, targetX, targetY, (const ZLQImageData*)&source, sourceX, sourceY);
}

shared_ptr<ZLImageData> QImageManager::createData() const {
	return new ZLQImageData();
}

void QImageManager::convertImageDirect(const std::string &stringData, ZLImageData &data) const {
	((ZLQImageData&)data).loadFromData((const unsigned char*)stringData.data(), stringData.length());
}
