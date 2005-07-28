/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <abstract/ZLZDecompressor.h>
#include <abstract/ZLFSManager.h>
#include <abstract/ZLInputStream.h>

#include "ZLImage.h"

void ZLBase64EncodedImage::decode() const {
	if ((myEncodedData.empty()) || (!myData.isNull())) {
		return;
	}

	size_t dataLength = myEncodedData.length();

	myData = new ZLString();
	myData->reserve(dataLength / 4 * 3);
	for (size_t pos = 0, dataPos = 0; pos < dataLength; dataPos += 3) {
		unsigned int sum = 0;
		for (int i = 0; (i < 4) && (pos < dataLength); pos++) {
			char encodedByte = myEncodedData[pos];
			unsigned int number = 0;
			if (('A' <= encodedByte) && (encodedByte <= 'Z')) {
				number = encodedByte - 'A';
			} else if (('a' <= encodedByte) && (encodedByte <= 'z')) {
				number = encodedByte - 'a' + 26;
			} else if (('0' <= encodedByte) && (encodedByte <= '9')) {
				number = encodedByte - '0' + 52;
			} else if (encodedByte == '+') {
				number = 62;
			} else if (encodedByte == '/') {
				number = 63;
			} else if (encodedByte == '=') {
				number = 64;
			} else {
				continue;
			}
			sum += number << (6 * (3 - i));
			i++;
		}
		for (int j = 2; j >= 0; j--) {
			(*myData)[dataPos + j] = sum % 256;
			sum >>= 8;
		}
	}
	myEncodedData.erase();
}


shared_ptr<ZLString> ZLZCompressedFileImage::data() const {
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
