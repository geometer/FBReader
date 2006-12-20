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

#include <ZLImage.h>

#include "GtkImageManager.h"

unsigned int GtkImageData::width() const {
	return (myPixbuf != 0) ? gdk_pixbuf_get_width(myPixbuf) : 0;
}

unsigned int GtkImageData::height() const {
	return (myPixbuf != 0) ? gdk_pixbuf_get_height(myPixbuf) : 0;
}

void GtkImageData::init(unsigned int width, unsigned int height) {
	myPixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, width, height);
	myImageData = gdk_pixbuf_get_pixels(myPixbuf);
	myRowStride = gdk_pixbuf_get_rowstride(myPixbuf);
}

void GtkImageData::setPosition(unsigned int x, unsigned int y) {
	myPosition = myImageData + 3 * x + myRowStride * y;
}

void GtkImageData::moveX(int delta) {
	myPosition += 3 * delta;
}

void GtkImageData::moveY(int delta) {
	myPosition += myRowStride * delta;
}

void GtkImageData::setPixel(unsigned char r, unsigned char g, unsigned char b) {
	myPosition[0] = r;
	myPosition[1] = g;
	myPosition[2] = b;
}

void GtkImageData::copyFrom(const ZLImageData &source, unsigned int sourceX, unsigned int sourceY, unsigned int targetX, unsigned int targetY) {
	gdk_pixbuf_copy_area(
		((const GtkImageData&)source).myPixbuf,
		sourceX, sourceY,
		source.width(), source.height(),
		myPixbuf,
		targetX, targetY
	);
}

shared_ptr<ZLImageData> GtkImageManager::createData() const {
	return new GtkImageData();
}

void GtkImageManager::convertImageDirect(const std::string &stringData, ZLImageData &data) const {
	GdkPixbufLoader *loader = gdk_pixbuf_loader_new();
	GError *error = 0;
  
	gdk_pixbuf_loader_write(loader, (const unsigned char*)stringData.data(), stringData.length(), &error);
	if (error == 0) {
		gdk_pixbuf_loader_close(loader, &error);
		if (error == 0) {
			((GtkImageData&)data).myPixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
			g_object_ref(((GtkImageData&)data).myPixbuf);
		}
	}
  
	if (error != 0) {
		g_error_free(error);
	}
	g_object_unref(loader);
}
