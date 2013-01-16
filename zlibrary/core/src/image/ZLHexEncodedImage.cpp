/*
 * Copyright (C) 2009-2013 Geometer Plus <contact@geometerplus.com>
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

ZLHexEncodedImage::ZLHexEncodedImage(shared_ptr<ZLMimeType> mimeType, shared_ptr<std::string> encodedData) :
	ZLSingleImage(mimeType),
	myEncodedData(encodedData),
	myData(0) {
}

void ZLHexEncodedImage::read() const {
	if (myEncodedData.isNull()) {
		return;
	}
	myData = new std::string();
	const std::size_t length = myEncodedData->length();
	myData->reserve(length / 2);

	int first = -1;
	for (unsigned int i = 0; i < length; i += 1) {
		const int digit = decode(myEncodedData->at(i));
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

const shared_ptr<std::string> ZLHexEncodedImage::stringData() const {
	if (myData.isNull()) {
		read();
	}
	return myData;
}
