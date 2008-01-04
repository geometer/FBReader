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

#include <ZLFile.h>
#include <ZLInputStream.h>
#include <ZLZDecompressor.h>
#include <ZLStringUtil.h>

#include "PluckerImages.h"
#include "DocDecompressor.h"

const shared_ptr<std::string> ZCompressedFileImage::stringData() const {
	shared_ptr<ZLInputStream> stream = ZLFile(myPath).inputStream();

	shared_ptr<std::string> imageData = new std::string();

	if (!stream.isNull() && stream->open()) {
		stream->seek(myOffset, false);
		ZLZDecompressor decompressor(myCompressedSize);

		static const size_t charBufferSize = 2048;
		char *charBuffer = new char[charBufferSize];
		std::vector<std::string> buffer;

		size_t s;
		do {
			s = decompressor.decompress(*stream, charBuffer, charBufferSize);
			if (s != 0) {
				buffer.push_back(std::string());
				buffer.back().append(charBuffer, s);
			}
		} while (s == charBufferSize);
		ZLStringUtil::append(*imageData, buffer);

		delete[] charBuffer;
	}

	return imageData;
}

const shared_ptr<std::string> DocCompressedFileImage::stringData() const {
	shared_ptr<ZLInputStream> stream = ZLFile(myPath).inputStream();

	shared_ptr<std::string> imageData = new std::string();

	if (!stream.isNull() && stream->open()) {
		stream->seek(myOffset, false);
		char *buffer = new char[65535];
		size_t uncompressedSize = DocDecompressor().decompress(*stream, buffer, myCompressedSize, 65535);
		imageData->append(buffer, uncompressedSize);
		delete[] buffer;
	}

	return imageData;
}

shared_ptr<const ZLImage> PluckerMultiImage::subImage(unsigned int row, unsigned int column) const {
	unsigned int index = row * myColumns + column;
	if (index >= myIds.size()) {
		return 0;
	}
	ZLImageMap::const_iterator entry = myImageMap.find(myIds[index]);
	return (entry != myImageMap.end()) ? entry->second : 0;
}
