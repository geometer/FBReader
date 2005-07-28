/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <iostream>

#include <abstract/ZLImage.h>

#include "GtkImageConverter.h"

std::map<const ZLImage*,GdkPixbuf*> GtkImageConverter::ourImageMap;

GdkPixbuf *GtkImageConverter::gtkImage(const ZLImage &image) {
	GdkPixbuf *imageRef = ourImageMap[&image];

	if (imageRef == 0) {
		shared_ptr<ZLString> imageData = image.data();
		if (!imageData.isNull() && !imageData->empty()) {
			if (image.mimeType() == "image/palm") {
				imageRef = gtkImageFromPalmImageFormat(*imageData);
			} else {
				GdkPixbufLoader *loader = gdk_pixbuf_loader_new();
				GError *error = 0;
      
				gdk_pixbuf_loader_write(loader, (const unsigned char*)imageData->data(), imageData->length(), &error);
				if (error == 0) {
					gdk_pixbuf_loader_close(loader, &error);
					if (error == 0) {
						imageRef = gdk_pixbuf_loader_get_pixbuf(loader);
						g_object_ref(imageRef);
					}
				}
      
				if (error != 0) {
					g_error_free(error);
				}
				g_object_unref(loader);
			}
		}
		ourImageMap[&image] = imageRef; 
	}

	return imageRef;
}

GdkPixbuf *GtkImageConverter::gtkImageFromPalmImageFormat(const ZLString &imageData) {
	GdkPixbuf *imageRef = 0;

	if (imageData.length() >= 16) {
		PalmImageHeader header(imageData);
		switch (header.CompressionType) {
			case 0x00: // scanline
				std::cerr << "scanline encoded images are not supported yet\n";
				break;
			case 0x01: // rle
				std::cerr << "rle encoded images are not supported yet\n";
				break;
			case 0x02: //packbits
				std::cerr << "packbits encoded images are not supported yet\n";
				break;
			case 0xFF: // none
				if (imageData.length() >= (size_t)header.BytesPerRow * header.Height + 16) {
					if ((header.BitsPerPixel != 1) &&
							(header.BitsPerPixel != 2) &&
							(header.BitsPerPixel != 4) &&
							(header.BitsPerPixel != 8) &&
							(header.BitsPerPixel != 16)) {
						std::cerr << "images with bpp = " << (int)header.BitsPerPixel << " are not supported\n";
						break;
					}
					imageRef = gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, header.Width, header.Height);

					if (header.BitsPerPixel == 16) {
						unsigned char redBits = imageData[16];
						unsigned char greenBits = imageData[17];
						unsigned char blueBits = imageData[18];
						const unsigned char *from = (const unsigned char*)imageData.data() + 24;
						guchar *to = gdk_pixbuf_get_pixels(imageRef);
						int rowstride = gdk_pixbuf_get_rowstride(imageRef);

						for (unsigned short i = 0; i < header.Height; i++) {
							const unsigned char *from_ptr = from + header.BytesPerRow * i;
							guchar *to_ptr = to + rowstride * i;
							for (unsigned short j = 0; j < header.BytesPerRow; j += 2) {
								unsigned short color = 256 * *from_ptr + *(from_ptr + 1);
								to_ptr[0] = color >> (16 - redBits);
								to_ptr[1] = (color >> blueBits) & ((1 << greenBits) - 1);
								to_ptr[2] = color & ((1 << blueBits) - 1);
							}
						}
						break;
					}
					const unsigned char *from = (const unsigned char*)imageData.data() + 16;
					guchar *to = gdk_pixbuf_get_pixels(imageRef);
					int rowstride = gdk_pixbuf_get_rowstride(imageRef);

					for (unsigned short i = 0; i < header.Height; i++) {
						const unsigned char *from_ptr = from + header.BytesPerRow * i;
						guchar *to_ptr = to + rowstride * i;
						switch (header.BitsPerPixel) {
							case 1:
								for (int j = 0; j < std::min(header.Width + 7, header.BytesPerRow * 8); j += 8, from_ptr++) {
									unsigned char len = std::min(8, header.Width - j);
									for (unsigned char k = 0; k < len; k++) {
										to_ptr[0] = to_ptr[1] = to_ptr[2] = (*from_ptr & (1 << k)) ? 0 : 255;
										to_ptr += 3;
									}
								}
								break;
							case 2:
								for (int j = 0; j < std::min(header.Width + 3, header.BytesPerRow * 4); j += 4, from_ptr++) {
									unsigned char len = std::min(4, header.Width - j);
									for (unsigned char k = 0; k < len; k++) {
										to_ptr[0] = to_ptr[1] = to_ptr[2] = 85 * (3 - (*from_ptr >> (6 - 2 * k)) & 0x3);
										to_ptr += 3;
									}
								}
								break;
							case 4:
								for (int j = 0; j < std::min(header.Width + 1, header.BytesPerRow * 2); j += 2, from_ptr++) {
									to_ptr[0] = to_ptr[1] = to_ptr[2] = 17 * (15 - (*from_ptr >> 4));
									to_ptr += 3;
									if (j != header.Width - 1) {
										to_ptr[0] = to_ptr[1] = to_ptr[2] = 17 * (15 - (*from_ptr & 0xF));
										to_ptr += 3;
									}
								}
								break;
							case 8:
								for (unsigned short j = 0; j < std::min(header.Width, header.BytesPerRow); j++, from_ptr++) {
									*(Color*)to_ptr = ourPalmImage8bitColormap[*from_ptr];
									to_ptr += 3;
								}
								break;
						}
					}
				}
				break;
			default: // unknown
				std::cerr << "unknown image encoding: " << (int)header.CompressionType << "\n";
				break;
		}
	}

	return imageRef;
}

void GtkImageConverter::clearCache() {
	for (std::map<const ZLImage*,GdkPixbuf*>::iterator it = ourImageMap.begin(); it != ourImageMap.end(); it++) {
		if (it->second != 0) {
			g_object_unref(it->second);
		}
	}
	ourImageMap.clear();
}
