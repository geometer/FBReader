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

#include "ZLWin32ImageManager.h"

ZLWin32ImageData::ZLWin32ImageData() : myWidth(0), myHeight(0), myArray(0) {
}

ZLWin32ImageData::~ZLWin32ImageData() {
	if (myArray != 0) {
		delete[] myArray;
	}
}

unsigned int ZLWin32ImageData::width() const {
	return myWidth;
}

unsigned int ZLWin32ImageData::height() const {
	return myHeight;
}

void ZLWin32ImageData::init(unsigned int width, unsigned int height) {
	myWidth = width;
	myHeight = height;
	myBytesPerLine = (3 * width + 3) >> 2 << 2;
	if (myArray != 0) {
		delete[] myArray;
	}
	myArray = new BYTE[myBytesPerLine * myHeight];
	myPixelPointer = myArray;

	myInfo.bmiHeader.biSize = sizeof(myInfo.bmiHeader);
	myInfo.bmiHeader.biWidth = width;
	myInfo.bmiHeader.biHeight = height;
	myInfo.bmiHeader.biPlanes = 1;
	myInfo.bmiHeader.biBitCount = 24;
	myInfo.bmiHeader.biCompression = BI_RGB;
	myInfo.bmiHeader.biSizeImage = 0;
	myInfo.bmiHeader.biXPelsPerMeter = 100; //?
	myInfo.bmiHeader.biYPelsPerMeter = 100; //? 
	myInfo.bmiHeader.biClrUsed = 0;
	myInfo.bmiHeader.biClrImportant = 0;
	myInfo.bmiColors[0].rgbBlue = 1;
	myInfo.bmiColors[0].rgbGreen = 1;
	myInfo.bmiColors[0].rgbRed = 1;
	myInfo.bmiColors[0].rgbReserved = 0;
}

void ZLWin32ImageData::setPosition(unsigned int x, unsigned int y) {
	myPixelPointer = myArray + myBytesPerLine * (myHeight - y - 1) + 3 * x;
}

void ZLWin32ImageData::moveX(int delta) {
	myPixelPointer += 3 * delta;
}

void ZLWin32ImageData::moveY(int delta) {
	myPixelPointer -= myBytesPerLine * delta;
}

void ZLWin32ImageData::setPixel(unsigned char r, unsigned char g, unsigned char b) {
	if (myArray != 0) {
		myPixelPointer[0] = b;
		myPixelPointer[1] = g;
		myPixelPointer[2] = r;
	}
}

void ZLWin32ImageData::copyFrom(const ZLImageData &source, unsigned int targetX, unsigned int targetY) {
	ZLWin32ImageData &win32source = (ZLWin32ImageData&)source;
	if ((myArray == 0) || (win32source.myArray == 0)) {
		return;
	}
	int height = source.height();
	int bytes = source.width() * 3;
	BYTE *src = win32source.myArray + win32source.myBytesPerLine * (win32source.myHeight - 1);
	BYTE *dst = myArray + (myHeight - targetY - 1) * myBytesPerLine + targetX * 3;
	for (int i = 0; i < height; ++i) {
		memcpy(dst, src, bytes);
		dst -= myBytesPerLine;
		src -= win32source.myBytesPerLine;
	}
}

const BYTE *ZLWin32ImageData::pixels() const {
	return myArray;
}

const BITMAPINFO &ZLWin32ImageData::info() const {
	return myInfo;
}

shared_ptr<ZLImageData> ZLWin32ImageManager::createData() const {
	return new ZLWin32ImageData();
}

void ZLWin32ImageManager::convertImageDirect(const std::string &stringData, ZLImageData &data) const {
	/*
	GdkPixbufLoader *loader = gdk_pixbuf_loader_new();
	GError *error = 0;
  
	gdk_pixbuf_loader_write(loader, (const unsigned char*)stringData.data(), stringData.length(), &error);
	if (error == 0) {
		gdk_pixbuf_loader_close(loader, &error);
		if (error == 0) {
			((ZLWin32ImageData&)data).myPixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
			g_object_ref(((ZLWin32ImageData&)data).myPixbuf);
		}
	}
  
	if (error != 0) {
		g_error_free(error);
	}
	g_object_unref(loader);
	*/
}
