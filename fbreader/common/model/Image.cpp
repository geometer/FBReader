/*
 * FBReader -- electronic book reader
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

#include "Image.h"

Image::Image(const std::string &mimeType) : myMimeType(mimeType) {
	myData = 0;
	myDataLen = 0;
}

Image::~Image() {
	if (myData != 0) {
		delete[] myData;
	}
}

void Image::decode() const {
	if ((myEncodedData.empty()) || (myData != 0)) {
		return;
	}

	int dataLength = myEncodedData.length();

	myData = new unsigned char[dataLength / 4 * 3 + 1];
	myDataLen = 0;
	for (int pos = 0; pos < dataLength; myDataLen += 3) {
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
			myData[myDataLen + j] = sum % 256;
			sum >>= 8;
		}
	}
	myDataLen -= 1;
	myEncodedData.erase();
}
