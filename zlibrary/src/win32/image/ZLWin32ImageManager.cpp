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

ZLWin32ImageData::ZLWin32ImageData() : myBitmap(0) {
}

ZLWin32ImageData::~ZLWin32ImageData() {
	if (myBitmap != 0) {
		DeleteBitmap(myBitmap);
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
	char *ptr = new char[10 * myWidth * myHeight];
	memset(ptr, 0, 10 * myWidth * myHeight);
	myBitmap = CreateBitmap(myWidth, myHeight, 3, 8, ptr);
}

void ZLWin32ImageData::setPosition(unsigned int x, unsigned int y) {
	//myPosition = myImageData + 3 * x + myRowStride * y;
}

void ZLWin32ImageData::moveX(int delta) {
	//myPosition += 3 * delta;
}

void ZLWin32ImageData::moveY(int delta) {
	//myPosition += myRowStride * delta;
}

void ZLWin32ImageData::setPixel(unsigned char r, unsigned char g, unsigned char b) {
	/*
	myPosition[0] = r;
	myPosition[1] = g;
	myPosition[2] = b;
	*/
}

void ZLWin32ImageData::copyFrom(const ZLImageData &source, unsigned int sourceX, unsigned int sourceY, unsigned int targetX, unsigned int targetY) {
	/*
	gdk_pixbuf_copy_area(
		((const ZLWin32ImageData&)source).myPixbuf,
		sourceX, sourceY,
		source.width(), source.height(),
		myPixbuf,
		targetX, targetY
	);
	*/
}

HBITMAP ZLWin32ImageData::bitmap() const {
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
