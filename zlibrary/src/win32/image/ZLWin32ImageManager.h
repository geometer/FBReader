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

#ifndef __ZLWIN32IMAGEMANAGER_H__
#define __ZLWIN32IMAGEMANAGER_H__

#include <windows.h>

#include <ZLImageManager.h>
#include <ZLImage.h>

class ZLWin32ImageData : public ZLImageData {

public:
	ZLWin32ImageData();
	~ZLWin32ImageData();

	unsigned int width() const;
	unsigned int height() const;

	void init(unsigned int width, unsigned int height);
	void setPosition(unsigned int x, unsigned int y);
	void moveX(int delta);
	void moveY(int delta);
	void setPixel(unsigned char r, unsigned char g, unsigned char b);

	void copyFrom(const ZLImageData &source, unsigned int targetX, unsigned int targetY);

	const BYTE *pixels() const;
	const BITMAPINFO &info() const;

private:
	unsigned int myWidth;
	unsigned int myHeight;
	unsigned int myBytesPerLine;

	BYTE *myArray;
	BYTE *myPixelPointer;

	BITMAPINFO myInfo;

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
};

#endif /* __ZLWIN32IMAGEMANAGER_H__ */
