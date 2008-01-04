/*
 * Copyright (C) 2007-2008 Geometer Plus <contact@geometerplus.com>
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

#include <gif_lib.h>

#include "ZLWin32ImageManager.h"

class GifReader {

public:
	static int inputFunction(GifFileType *fileType, GifByteType *buffer, int len);

public:
	GifReader(const std::string &data);

private:
	const std::string &myData;
	size_t myOffset;
};

GifReader::GifReader(const std::string &data) : myData(data), myOffset(0) {
}

int GifReader::inputFunction(GifFileType *fileType, GifByteType *buffer, int len) {
	GifReader &reader = *(GifReader*)fileType->UserData;
	len = std::min(len, (int)(reader.myData.length() - reader.myOffset));
	memcpy(buffer, reader.myData.data() + reader.myOffset, len);
	reader.myOffset += len;
	return len;
}

inline static void copyLine(ColorMapObject &cm, byte *&ptr, ZLWin32ImageData &data, int w, int transparent) {
	if (transparent != -1) {
		for (int j = 0; j < w; ++j) {
			GifColorType &color = cm.Colors[*ptr];
			data.setPixel(color.Red, color.Green, color.Blue, (*ptr++ == transparent) ? 0 : 255);
			data.moveX(1);
		}
	} else {
		for (int j = 0; j < w; ++j) {
			GifColorType &color = cm.Colors[*ptr++];
			data.setPixel(color.Red, color.Green, color.Blue);
			data.moveX(1);
		}
	}
}

bool ZLWin32ImageManager::gifConvert(const std::string &stringData, ZLWin32ImageData &data) const {
	GifReader reader(stringData);

	GifFileType *gif = DGifOpen(&reader, GifReader::inputFunction);
	if (gif == 0) {
		return false;
	}

	if (DGifSlurp(gif) != GIF_OK) {
		DGifCloseFile(gif);
		return false;
	}

	ColorMapObject* colormap = gif->SColorMap;
	if ((colormap == 0) ||
			(gif->ImageCount < 1) ||
			(colormap->ColorCount != (1 << colormap->BitsPerPixel))) {
		DGifCloseFile(gif);
		return false;
	}

	SavedImage* gif_image = gif->SavedImages;
 
	const int width = gif->SWidth;
	const int height = gif->SHeight;

	int transparent = -1;
	for (int i = 0; i < gif_image->ExtensionBlockCount; ++i) {
		ExtensionBlock* eb = gif_image->ExtensionBlocks + i;
		if (eb->Function == 0xF9 && 
				eb->ByteCount == 4) {
			if ((eb->Bytes[0] & 1) == 1) {
				transparent = (unsigned byte)eb->Bytes[3];
			}
		}
	}

	data.init(width, height, (transparent != -1), 0);

	byte *ptr = gif_image->RasterBits;
	if (gif->Image.Interlace) {
		for (int i = 0; i < height; i += 8) {
			data.setPosition(0, i);
			copyLine(*colormap, ptr, data, width, transparent);
		}
		for (int i = 4; i < height; i += 8) {
			data.setPosition(0, i);
			copyLine(*colormap, ptr, data, width, transparent);
		}
		for (int i = 2; i < height; i += 4) {
			data.setPosition(0, i);
			copyLine(*colormap, ptr, data, width, transparent);
		}
		for (int i = 1; i < height; i += 2) {
			data.setPosition(0, i);
			copyLine(*colormap, ptr, data, width, transparent);
		}
	} else {
		for (int i = 0; i < height; ++i) {
			data.setPosition(0, i);
			copyLine(*colormap, ptr, data, width, transparent);
		}
	}

	DGifCloseFile(gif);
	return true;
}
