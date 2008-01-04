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

#include <png.h>

#include "ZLWin32ImageManager.h"

class PngReader {

public:
	PngReader(const std::string &data) : myData(data), myOffset(0) {
	}
	bool read(png_bytep buffer, png_size_t length) {
		if (myOffset + length > myData.length()) {
			return false;
		}
		memcpy(buffer, myData.data() + myOffset, length);
		myOffset += length;
		return true;
	}

private:
	const std::string &myData;
	size_t myOffset;
};

static void pngReadFunction(png_structp png_ptr, png_bytep data, png_size_t length) {
	PngReader &reader = *(PngReader*)png_get_io_ptr(png_ptr);
	if (!reader.read(data, length)) {
		png_error(png_ptr, "Read Error");
	}
}

bool ZLWin32ImageManager::pngConvert(const std::string &stringData, ZLWin32ImageData &data) const {
	if (!png_check_sig((png_byte*)stringData.data(), 8)) {
		return false;
	}

	png_structp pngStructure = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (pngStructure == 0) {
		return true;
	}

	png_infop pngInfo = png_create_info_struct(pngStructure);
	if (pngInfo == 0) {
		png_destroy_read_struct(&pngStructure, 0, 0);
		return true;
	}

	if (setjmp(png_jmpbuf(pngStructure))) {
		png_destroy_read_struct(&pngStructure, &pngInfo, 0);
		return true;
	}

	PngReader reader(stringData);
	png_set_read_fn(pngStructure, (void*)&reader, pngReadFunction);
	png_read_info(pngStructure, pngInfo);
	png_uint_32 width;
	png_uint_32 height;
	int bitDepth;
	int colorType;
	png_get_IHDR(pngStructure, pngInfo, &width, &height, &bitDepth, &colorType, 0, 0, 0);

	if (colorType == PNG_COLOR_TYPE_PALETTE) {
		png_set_expand(pngStructure);
	}
	if ((colorType == PNG_COLOR_TYPE_GRAY) && (bitDepth < 8)) {
		png_set_expand(pngStructure);
	}
	if (png_get_valid(pngStructure, pngInfo, PNG_INFO_tRNS)) {
		png_set_expand(pngStructure);
	}
	if (bitDepth == 16) {
		png_set_strip_16(pngStructure);
	}
	if ((colorType == PNG_COLOR_TYPE_GRAY) || (colorType == PNG_COLOR_TYPE_GRAY_ALPHA)) {
		png_set_gray_to_rgb(pngStructure);
	}

	double gamma;
	if (png_get_gAMA(pngStructure, pngInfo, &gamma)) {
		png_set_gamma(pngStructure, 2.2, gamma);
	}
	png_read_update_info(pngStructure, pngInfo);

	unsigned int bpp = png_get_channels(pngStructure, pngInfo);
	if ((bpp != 3) && (bpp != 4)) {
		png_destroy_read_struct(&pngStructure, &pngInfo, 0);
		return true;
	}

	unsigned int bpl = png_get_rowbytes(pngStructure, pngInfo);
	data.init(width, height, bpp == 4, bpl);
	
	png_bytep *rowPointers = new png_bytep[height];
	for (png_uint_32 i = 0; i < height; ++i) {
		rowPointers[i] = data.myArray + (height - i - 1) * data.myBytesPerLine;
	}
	png_read_image(pngStructure, rowPointers);
	delete[] rowPointers;

	data.bgr2rgb();

	png_destroy_read_struct(&pngStructure, &pngInfo, 0);
	return true;
}
