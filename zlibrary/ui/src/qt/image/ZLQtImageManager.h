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

#ifndef __ZLQTIMAGEMANAGER_H__
#define __ZLQTIMAGEMANAGER_H__

#include <map>
#include <qimage.h>

#include <ZLImageManager.h>

class ZLImage;

class ZLQtImageData : public ZLImageData, public QImage {

public:
	ZLQtImageData() : QImage(), myX(0), myY(0) {}
	~ZLQtImageData() {}

	unsigned int width() const { return QImage::width(); }
	unsigned int height() const { return QImage::height(); }

	void init(unsigned int width, unsigned int height);
	void setPosition(unsigned int x, unsigned int y);
	void moveX(int delta);
	void moveY(int delta);
	void setPixel(unsigned char r, unsigned char g, unsigned char b);

	void copyFrom(const ZLImageData &source, unsigned int targetX, unsigned int targetY);

private:
	unsigned int myX, myY;

friend class ZLQtImageManager;
};

class ZLQtImageManager : public ZLImageManager {

public:
	static void createInstance() { ourInstance = new ZLQtImageManager(); }

private:
	ZLQtImageManager() {}
	
protected:
	~ZLQtImageManager() {}
	shared_ptr<ZLImageData> createData() const;
	void convertImageDirect(const std::string &stringData, ZLImageData &imageData) const;
};

#endif /* __ZLQTIMAGEMANAGER_H__ */
