/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

#include <ZLStringUtil.h>
#include <ZLInputStream.h>
#include <ZLFile.h>

#include "ZLHexEncodedImage.h"

inline static int decode(char b) {
	switch (b) {
		default:
			return -1;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			return b - '0';
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
			return b - 'A' + 10;
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
			return b - 'a' + 10;
	}
}

void ZLHexEncodedImage::read() const {
	shared_ptr<ZLInputStream> stream = ZLFile(myFileName).inputStream();
	if (stream.isNull() || !stream->open()) {
		return;
	}
	myData = new std::string();
	myData->reserve(myLength / 2);
	stream->seek(myStartOffset, false);
	const size_t bufferSize = 1024;
	char *buffer = new char[bufferSize];

	int first = -1;
	for (unsigned int i = 0; i < myLength; i += bufferSize) {
		size_t toRead = std::min(bufferSize, myLength - i);
		if (stream->read(buffer, toRead) != toRead) {
			break;
		}
		for (size_t j = 0; j < toRead; j += 1) {
			const int digit = decode(buffer[j]);
			if (digit != -1) {
				if (first == -1) {
					first = digit;
				} else {
					*myData += (char)((first << 4) + digit);
					first = -1;
				}
			}
		}
	}
	delete[] buffer;
	stream->close();
}

const shared_ptr<std::string> ZLHexEncodedImage::stringData() const {
	if (myData.isNull()) {
		read();
	}
	return myData;
}
