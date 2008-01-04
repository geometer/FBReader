/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLWIN32IMAGEMANAGER_H__
#define __ZLWIN32IMAGEMANAGER_H__

#include <windows.h>

#include <ZLImageManager.h>
#include <ZLImage.h>
#include <ZLColor.h>

class ZLWin32ImageData : public ZLImageData {

public:
	ZLWin32ImageData();
	~ZLWin32ImageData();

	unsigned int width() const;
	unsigned int height() const;

	void init(unsigned int width, unsigned int height);
	void init(unsigned int width, unsigned int height, bool hasAlpha, unsigned int bytesPerLine);
	void setPosition(unsigned int x, unsigned int y);
	void moveX(int delta);
	void moveY(int delta);
	void setPixel(unsigned char r, unsigned char g, unsigned char b);
	void setPixel(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

	void copyFrom(const ZLImageData &source, unsigned int targetX, unsigned int targetY);

	const BYTE *pixels(ZLColor bgColor) const;
	const BITMAPINFO *info() const;

private:
	void bgr2rgb();
	void clear();

private:
	unsigned int myWidth;
	unsigned int myHeight;
	unsigned int myBytesPerPixel;
	unsigned int myBytesPerLine;

	BYTE *myArray;
	mutable BYTE *myArrayWithoutAlpha;
	mutable ZLColor myBackgroundColor;
	BYTE *myPixelPointer;

	BITMAPINFO *myInfo;

friend class ZLWin32ImageManager;
};

class ZLWin32ImageManager : public ZLImageManager {

public:
	static void createInstance() { ourInstance = new ZLWin32ImageManager(); }

private:
	ZLWin32ImageManager() {}
	
protected:
	shared_ptr<ZLImageData> createData() const;
	void convertImageDirect(const std::string &stringData, ZLImageData &imageData) const;

private:
	bool bmpConvert(const std::string &stringData, ZLWin32ImageData &imageData) const;
	bool pngConvert(const std::string &stringData, ZLWin32ImageData &imageData) const;
	bool jpegConvert(const std::string &stringData, ZLWin32ImageData &imageData) const;
	bool gifConvert(const std::string &stringData, ZLWin32ImageData &imageData) const;
	bool tiffConvert(const std::string &stringData, ZLWin32ImageData &imageData) const;
};

inline void ZLWin32ImageData::setPosition(unsigned int x, unsigned int y) {
	myPixelPointer = myArray + myBytesPerLine * (myHeight - y - 1) + myBytesPerPixel * x;
}

inline void ZLWin32ImageData::moveX(int delta) {
	myPixelPointer += myBytesPerPixel * delta;
}

inline void ZLWin32ImageData::moveY(int delta) {
	myPixelPointer -= myBytesPerLine * delta;
}

inline void ZLWin32ImageData::setPixel(unsigned char r, unsigned char g, unsigned char b) {
	if (myArray != 0) {
		myPixelPointer[0] = b;
		myPixelPointer[1] = g;
		myPixelPointer[2] = r;
		if (myBytesPerPixel == 4) {
			myPixelPointer[3] = 255;
		}
	}
}

inline void ZLWin32ImageData::setPixel(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	if (myArray != 0) {
		myPixelPointer[0] = b;
		myPixelPointer[1] = g;
		myPixelPointer[2] = r;
		myPixelPointer[3] = a;
	}
}

#endif /* __ZLWIN32IMAGEMANAGER_H__ */
