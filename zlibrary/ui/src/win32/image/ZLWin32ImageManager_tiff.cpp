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

#include <tiffio.h>

#include "ZLWin32ImageManager.h"

class TIFFReader {

public:
	static tsize_t read(thandle_t readerPtr, tdata_t buffer, tsize_t len);
	static tsize_t write(thandle_t readerPtr, tdata_t buffer, tsize_t len);
	static toff_t seek(thandle_t readerPtr, toff_t offset, int);
	static int close(thandle_t readerPtr);
	static toff_t size(thandle_t readerPtr);
	static int map(thandle_t readerPtr, tdata_t*, toff_t*);
	static void unmap(thandle_t readerPtr, tdata_t, toff_t);

public:
	TIFFReader(const std::string &data);

private:
	const std::string &myData;
	size_t myOffset;
};

TIFFReader::TIFFReader(const std::string &data) : myData(data), myOffset(0) {
}

tsize_t TIFFReader::read(thandle_t readerPtr, tdata_t buffer, tsize_t len) {
	TIFFReader &reader = *(TIFFReader*)readerPtr;
	len = std::min(len, (tsize_t)(reader.myData.length() - reader.myOffset));
	if (len > 0) {
		memcpy(buffer, reader.myData.data() + reader.myOffset, len);
		reader.myOffset += len;
	}
	return len;
}

tsize_t TIFFReader::write(thandle_t, tdata_t, tsize_t) {
	// never used
	return -1;
}

toff_t TIFFReader::seek(thandle_t readerPtr, toff_t offset, int whence) {
	TIFFReader &reader = *(TIFFReader*)readerPtr;
	int newOffset = offset;
	switch (whence) {
		case SEEK_SET:
			break;
		case SEEK_END:
			newOffset += reader.myData.length();
			break;
		case SEEK_CUR:
			newOffset += reader.myOffset;
			break;
	}
	if ((newOffset < 0) || (newOffset >= (int)reader.myData.length())) {
		return (toff_t)-1;
	}
	reader.myOffset = newOffset;
	return reader.myOffset;
}

int TIFFReader::close(thandle_t) {
	return 0;
}

int TIFFReader::map(thandle_t readerPtr, tdata_t *buffer, toff_t *size) {
	TIFFReader &reader = *(TIFFReader*)readerPtr;
	*buffer = (tdata_t)reader.myData.data();
	*size = (toff_t)reader.myData.length();
	return 1;
}

void TIFFReader::unmap(thandle_t, tdata_t, toff_t) {
}

toff_t TIFFReader::size(thandle_t readerPtr) {
	return ((TIFFReader*)readerPtr)->myData.length();
}

bool ZLWin32ImageManager::tiffConvert(const std::string &stringData, ZLWin32ImageData &data) const {
	TIFFReader reader(stringData);
	TIFF *tiff = TIFFClientOpen("ZLWin32ImageManager", "rM", &reader, TIFFReader::read, TIFFReader::write, TIFFReader::seek, TIFFReader::close, TIFFReader::size, TIFFReader::map, TIFFReader::unmap);
	if (tiff == 0) {
		return false;
	}

	int width, height;
	if (!TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width) ||
			!TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height)) {
		TIFFClose(tiff);
		return false;
	}

	data.init(width, height, true, 0);

	TIFFReadRGBAImage(tiff, width, height, (uint32*)data.myArray, 1);
	data.bgr2rgb();

	TIFFClose(tiff);
	return true;
}
