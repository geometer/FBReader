/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __QIMAGEMANAGER_H__
#define __QIMAGEMANAGER_H__

#include <map>
#include <qimage.h>

#include "../../abstract/image/ZLImageManager.h"

class ZLImage;

class ZLQImageData : public ZLImageData, public QImage {

public:
	ZLQImageData() : QImage(), myX(0), myY(0) {}
	~ZLQImageData() {}

	unsigned int width() const { return QImage::width(); }
	unsigned int height() const { return QImage::height(); }

	void init(unsigned int width, unsigned int height);
	void setPosition(unsigned int x, unsigned int y);
	void moveX(int delta);
	void moveY(int delta);
	void setPixel(unsigned char r, unsigned char g, unsigned char b);

	void copyFrom(const ZLImageData &source, unsigned int sourceX, unsigned int sourceY, unsigned int targetX, unsigned int targetY);

private:
	unsigned int myX, myY;

friend class QImageManager;
};

class QImageManager : public ZLImageManager {

public:
	static void createInstance() { ourInstance = new QImageManager(); }

private:
	QImageManager() {}
	
protected:
	~QImageManager() {}
	shared_ptr<ZLImageData> createData() const;
	void convertImageDirect(const std::string &stringData, ZLImageData &imageData) const;
};

#endif /* __QIMAGEMANAGER_H__ */
