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

#ifndef __IMAGECONVERTER_H__
#define __IMAGECONVERTER_H__

class ZLString;

class ImageConverter {

protected:
	struct Color {
  	unsigned char Red;
  	unsigned char Green;
  	unsigned char Blue;
	};

	struct PalmImageHeader {
		PalmImageHeader(const ZLString &str);
		
		unsigned short Width;
		unsigned short Height;
		unsigned short BytesPerRow;
		unsigned short Flags;
		unsigned char BitsPerPixel;
		unsigned char CompressionType;
	};

	static Color ourPalmImage8bitColormap[256];
};

#endif /* __IMAGECONVERTER_H__ */
