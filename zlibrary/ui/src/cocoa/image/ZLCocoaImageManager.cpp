/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#include "ZLCocoaImageManager.h"

unsigned int ZLCocoaImageData::width() const {
	return 50;
}

unsigned int ZLCocoaImageData::height() const {
	return 50;
}

void ZLCocoaImageData::init(unsigned int width, unsigned int height) {
	// TODO: implement
}

void ZLCocoaImageData::setPosition(unsigned int x, unsigned int y) {
	// TODO: implement
}

void ZLCocoaImageData::moveX(int delta) {
	// TODO: implement
}

void ZLCocoaImageData::moveY(int delta) {
	// TODO: implement
}

void ZLCocoaImageData::setPixel(unsigned char r, unsigned char g, unsigned char b) {
	// TODO: implement
}

void ZLCocoaImageData::copyFrom(const ZLImageData &source, unsigned int targetX, unsigned int targetY) {
	// TODO: implement
}

shared_ptr<ZLImageData> ZLCocoaImageManager::createData() const {
	return new ZLCocoaImageData();
}

void ZLCocoaImageManager::convertImageDirect(const std::string &stringData, ZLImageData &data) const {
	// TODO: implement
	/*
	GdkPixbufLoader *loader = gdk_pixbuf_loader_new();
	GError *error = 0;
  
	gdk_pixbuf_loader_write(loader, (const unsigned char*)stringData.data(), stringData.length(), &error);
	if (error == 0) {
		gdk_pixbuf_loader_close(loader, &error);
		if (error == 0) {
			((ZLCocoaImageData&)data).myPixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
			g_object_ref(((ZLCocoaImageData&)data).myPixbuf);
		}
	}
  
	if (error != 0) {
		g_error_free(error);
	}
	g_object_unref(loader);
	*/
}
