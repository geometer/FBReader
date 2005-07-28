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

#include "QImageConverter.h"

std::map<const ZLImage*,QImage*> QImageConverter::ourImageMap;

QImage *QImageConverter::qImage(const ZLImage &image) {
	QImage *imageRef = ourImageMap[&image];
	if (imageRef == NULL) {
		shared_ptr<ZLString> imageData = image.data();
		if (image.mimeType() == "image/palm") {
			imageRef = qImageFromPalmImageFormat(*imageData);
		} else {
			imageRef = new QImage();
			imageRef->loadFromData((const unsigned char*)imageData->data(), imageData->length());
		}
		ourImageMap[&image] = imageRef;
	}
	return imageRef;
}

QImage *QImageConverter::qImageFromPalmImageFormat(const ZLString &imageData) {
	QImage *imageRef = 0;

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
					imageRef = new QImage(header.Width, header.Height, 32);

					if (header.BitsPerPixel == 16) {
						unsigned char redBits = imageData[16];
						unsigned char greenBits = imageData[17];
						unsigned char blueBits = imageData[18];
						const unsigned char *from = (const unsigned char*)imageData.data() + 24;
						for (unsigned short i = 0; i < header.Height; i++) {
							const unsigned char *from_ptr = from + header.BytesPerRow * i;
							for (unsigned short j = 0; j < header.BytesPerRow; j += 2) {
								unsigned short color = 256 * *from_ptr + *(from_ptr + 1);
								unsigned char red = color >> (16 - redBits);
								unsigned char green = (color >> blueBits) & ((1 << greenBits) - 1);
								unsigned char blue = color & ((1 << blueBits) - 1);
								imageRef->setPixel(j, i, qRgb(red, green, blue));
							}
						}
						break;
					}

					const unsigned char *from = (const unsigned char*)imageData.data() + 16;
					for (unsigned short i = 0; i < header.Height; i++) {
						const unsigned char *from_ptr = from + header.BytesPerRow * i;
						switch (header.BitsPerPixel) {
							case 1:
								for (int j = 0; j < std::min(header.Width + 7, header.BytesPerRow * 8); j += 8, from_ptr++) {
									unsigned char len = std::min(8, header.Width - j);
									for (unsigned char k = 0; k < len; k++) {
										unsigned char color = (*from_ptr & (1 << k)) ? 0 : 255;
										imageRef->setPixel(j + k, i, qRgb(color, color, color));
									}
								}
								break;
							case 2:
								for (int j = 0; j < std::min(header.Width + 3, header.BytesPerRow * 4); j += 4, from_ptr++) {
									unsigned char len = std::min(4, header.Width - j);
									for (unsigned char k = 0; k < len; k++) {
										unsigned char color = 85 * (3 - (*from_ptr >> (6 - 2 * k)) & 0x3);
										imageRef->setPixel(j + k, i, qRgb(color, color, color));
									}
								}
								break;
							case 4:
								for (int j = 0; j < std::min(header.Width + 1, header.BytesPerRow * 2); j += 2, from_ptr++) {
									unsigned char color = 17 * (15 - (*from_ptr >> 4));
									imageRef->setPixel(j, i, qRgb(color, color, color));
									if (j != header.Width - 1) {
										color = 17 * (15 - (*from_ptr >> 4));
										imageRef->setPixel(j + 1, i, qRgb(color, color, color));
									}
								}
								break;
							case 8:
								for (unsigned short j = 0; j < std::min(header.Width, header.BytesPerRow); j++, from_ptr++) {
									Color &color = ourPalmImage8bitColormap[*from_ptr];
									imageRef->setPixel(j, i, qRgb(color.Red, color.Green, color.Blue));
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

void QImageConverter::clearCache() {
	for (std::map<const ZLImage*,QImage*>::iterator it = ourImageMap.begin(); it != ourImageMap.end(); it++) {
		if (it->second != 0) {
			delete it->second;
		}
	}
	ourImageMap.clear();
}
