/*
 * Copyright (C) 2009-2012 Geometer Plus <contact@geometerplus.com>
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

#include "ZLHexInputStream.h"

static const size_t BUFFER_SIZE = 32768;

ZLHexInputStream::ZLHexInputStream(shared_ptr<ZLInputStream> base) :
	myBaseStream(base) {
	myBufferOffset = 0;
	myBufferLength = 0;
	myBuffer = new char[BUFFER_SIZE];
}

ZLHexInputStream::~ZLHexInputStream() {
	delete[] myBuffer;
}

bool ZLHexInputStream::open() {
	return myBaseStream->open();
}

int ZLHexInputStream::read() {
	int first = -1;
	while (myBufferLength >= 0) {
		while (myBufferLength-- > 0) {
			const int digit = decode(myBuffer[myBufferOffset++]);
			if (digit != -1) {
				if (first == -1) {
					first = digit;
				} else {
					return (first << 4) + digit;
				}
			}
		}
		fillBuffer();
	}
	return -1;
}

size_t ZLHexInputStream::read(char *buffer, size_t maxSize) {
	int offset = myBufferOffset;
	int available = myBufferLength;
	int first = -1;
	for (size_t ready = 0; ready < maxSize;) {
		while (ready < maxSize && available-- > 0) {
			const int digit = decode(myBuffer[offset++]);
			if (digit != -1) {
				if (first == -1) {
					first = digit;
				} else {
					buffer[ready++] = (char)((first << 4) + digit);
					first = -1;
				}
			}
		}
		if (ready < maxSize) {
			if (!fillBuffer()) {
				return ready;
			}
			available = myBufferLength;
			offset = 0;
		}
	}
	myBufferLength = available;
	myBufferOffset = offset;
	return maxSize;
}

void ZLHexInputStream::close() {
	return myBaseStream->close();
}

void ZLHexInputStream::seek(int offset, bool absoluteOffset) {
	myBaseStream->seek(offset, absoluteOffset);
}

size_t ZLHexInputStream::offset() const {
	return myBaseStream->offset();
}

size_t ZLHexInputStream::sizeOfOpened() {
	return myBaseStream->sizeOfOpened();
}

bool ZLHexInputStream::fillBuffer() {
	myBufferLength = myBaseStream->read(myBuffer, BUFFER_SIZE);
	myBufferOffset = 0;
	if (myBufferLength == 0) {
		return false;
	}
	return true;
}

int ZLHexInputStream::decode(char b) {
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
