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

#include "ZLGtkImageManager.h"

unsigned int ZLGtkImageData::width() const {
	return (myPixbuf != 0) ? gdk_pixbuf_get_width(myPixbuf) : 0;
}

unsigned int ZLGtkImageData::height() const {
	return (myPixbuf != 0) ? gdk_pixbuf_get_height(myPixbuf) : 0;
}

void ZLGtkImageData::init(unsigned int width, unsigned int height) {
	myPixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, width, height);
	myImageData = gdk_pixbuf_get_pixels(myPixbuf);
	myRowStride = gdk_pixbuf_get_rowstride(myPixbuf);
}

void ZLGtkImageData::setPosition(unsigned int x, unsigned int y) {
	myPosition = myImageData + 3 * x + myRowStride * y;
}

void ZLGtkImageData::moveX(int delta) {
	myPosition += 3 * delta;
}

void ZLGtkImageData::moveY(int delta) {
	myPosition += myRowStride * delta;
}

void ZLGtkImageData::setPixel(unsigned char r, unsigned char g, unsigned char b) {
	myPosition[0] = r;
	myPosition[1] = g;
	myPosition[2] = b;
}

void ZLGtkImageData::copyFrom(const ZLImageData &source, unsigned int targetX, unsigned int targetY) {
	gdk_pixbuf_copy_area(
		((const ZLGtkImageData&)source).myPixbuf,
		0, 0,
		source.width(), source.height(),
		myPixbuf,
		targetX, targetY
	);
}

shared_ptr<ZLImageData> ZLGtkImageManager::createData() const {
	return new ZLGtkImageData();
}

void ZLGtkImageManager::convertImageDirect(const std::string &stringData, ZLImageData &data) const {
	GdkPixbufLoader *loader = gdk_pixbuf_loader_new();
	GError *error = 0;
  
	gdk_pixbuf_loader_write(loader, (const unsigned char*)stringData.data(), stringData.length(), &error);
	if (error == 0) {
		gdk_pixbuf_loader_close(loader, &error);
		if (error == 0) {
			((ZLGtkImageData&)data).myPixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
			g_object_ref(((ZLGtkImageData&)data).myPixbuf);
		}
	}
  
	if (error != 0) {
		g_error_free(error);
	}
	g_object_unref(loader);
}
