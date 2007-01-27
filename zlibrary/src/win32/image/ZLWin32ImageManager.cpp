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

#include <windowsx.h>

#include "ZLWin32ImageManager.h"

ZLWin32ImageData::ZLWin32ImageData() : myMode(MODE_NULL), myWidth(0), myHeight(0), myBitmap(0), myArray(0) {
}

ZLWin32ImageData::~ZLWin32ImageData() {
	setMode(MODE_NULL);
}

void ZLWin32ImageData::setMode(Mode mode) const {
	if (mode == myMode) {
		return;
	}
	switch (mode) {
		case MODE_NULL:
		case MODE_SIZE:
			if (myBitmap != 0) {
				DeleteBitmap(myBitmap);
				myBitmap = 0;
			}
			if (myArray != 0) {
				delete[] myArray;
				myArray = 0;
			}
			break;
		case MODE_BITMAP:
			if (myMode != MODE_ARRAY) {
				return;
			}
			myBitmap = CreateBitmap(myWidth, myHeight, 1, 24, myArray);
			delete[] myArray;
			myArray = 0;
			break;
		case MODE_ARRAY:
			switch (myMode) {
				case MODE_SIZE:
					myArray = new BYTE[myBytesPerLine * myHeight];
					myPixelPointer = myArray;
					break;
				case MODE_ARRAY:
					myArray = new BYTE[myBytesPerLine * myHeight];
					myPixelPointer = myArray;
					GetBitmapBits(myBitmap, myBytesPerLine * myHeight, myArray);
					DeleteBitmap(myBitmap);
					myBitmap = 0;
					break;
				default:
					return;
			}
			break;
	}
	myMode = mode;
}

unsigned int ZLWin32ImageData::width() const {
	return myWidth;
}

unsigned int ZLWin32ImageData::height() const {
	return myHeight;
}

void ZLWin32ImageData::init(unsigned int width, unsigned int height) {
	setMode(MODE_SIZE);
	myWidth = width;
	myHeight = height;
	myBytesPerLine = (3 * width + 1) >> 1 << 1;
}

void ZLWin32ImageData::setPosition(unsigned int x, unsigned int y) {
	if (myMode != MODE_ARRAY) {
		setMode(MODE_ARRAY);
	}
	myPixelPointer = myArray + myBytesPerLine * y + 3 * x;
}

void ZLWin32ImageData::moveX(int delta) {
	if (myMode != MODE_ARRAY) {
		setMode(MODE_ARRAY);
	}
	myPixelPointer += 3 * delta;
}

void ZLWin32ImageData::moveY(int delta) {
	if (myMode != MODE_ARRAY) {
		setMode(MODE_ARRAY);
	}
	myPixelPointer += myBytesPerLine * delta;
}

void ZLWin32ImageData::setPixel(unsigned char r, unsigned char g, unsigned char b) {
	if (myMode == MODE_ARRAY) {
		myPixelPointer[0] = b;
		myPixelPointer[1] = g;
		myPixelPointer[2] = r;
	}
}

void ZLWin32ImageData::copyFrom(const ZLImageData &source, unsigned int targetX, unsigned int targetY) {
	setMode(MODE_ARRAY);
	if (myMode != MODE_ARRAY) {
		return;
	}
	ZLWin32ImageData &win32source = (ZLWin32ImageData&)source;
	win32source.setMode(MODE_ARRAY);
	if (win32source.myMode != MODE_ARRAY) {
		return;
	}
	int height = source.height();
	int bytes = source.width() * 3;
	for (int i = 0; i < height; ++i) {
		memcpy(myArray + (targetY + i) * myBytesPerLine + targetX * 3, win32source.myArray + i * win32source.myBytesPerLine, bytes);
	}
}

HBITMAP ZLWin32ImageData::bitmap() const {
	setMode(MODE_BITMAP);
	return myBitmap;
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
