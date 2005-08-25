/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <abstract/ZLFSManager.h>
#include <abstract/ZLInputStream.h>
#include <abstract/ZLZDecompressor.h>

#include "PluckerImages.h"
#include "DocDecompressor.h"

const shared_ptr<ZLString> FileImage::stringData() const {
	shared_ptr<ZLInputStream> stream = ZLFile(myPath).inputStream();

	shared_ptr<ZLString> imageData = new ZLString();

	if (!stream.isNull() && stream->open()) {
		stream->seek(myOffset);
		imageData->reserve(mySize);
		stream->read(imageData->data(), mySize);
	}

	return imageData;
}

const shared_ptr<ZLString> ZCompressedFileImage::stringData() const {
	shared_ptr<ZLInputStream> stream = ZLFile(myPath).inputStream();

	shared_ptr<ZLString> imageData = new ZLString();

	if (!stream.isNull() && stream->open()) {
		stream->seek(myOffset);
		ZLZDecompressor decompressor(myCompressedSize);
		static const size_t charBufferSize = 2048;
		ZLString charBuffer;
		charBuffer.reserve(charBufferSize);
		ZLStringBuffer buffer;

		size_t s;
		do {
			s = decompressor.decompress(*stream, charBuffer.data(), charBufferSize);
			if (s != 0) {
				buffer.push_back(ZLString());
				buffer.back().append(charBuffer.data(), s);
			}
		} while (s == charBufferSize);
		*imageData += buffer;
	}

	return imageData;
}

const shared_ptr<ZLString> DocCompressedFileImage::stringData() const {
	shared_ptr<ZLInputStream> stream = ZLFile(myPath).inputStream();

	shared_ptr<ZLString> imageData = new ZLString();

	if (!stream.isNull() && stream->open()) {
		stream->seek(myOffset);
		char *buffer = new char[65535];
		size_t uncompressedSize = DocDecompressor().decompress(*stream, buffer, myCompressedSize);
		imageData->append(buffer, uncompressedSize);
		delete[] buffer;
	}

	return imageData;
}

const ZLImage *PluckerMultiImage::subImage(unsigned int row, unsigned int column) const {
	unsigned int index = row * myColumns + column;
	if (index >= myIds.size()) {
		return 0;
	}
	ImageMap::const_iterator entry = myImageMap.find(myIds[index]);
	return (entry != myImageMap.end()) ? entry->second : 0;
}

