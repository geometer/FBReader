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

#include "ZLBase64InputStream.h"

static const size_t BUFFER_SIZE = 32768;

ZLBase64InputStream::ZLBase64InputStream(shared_ptr<ZLInputStream> base) :
	myBaseStream(base) {
	myBufferOffset = 0;
	myBufferLength = 0;
	myDecoded0 = -1;
	myDecoded1 = -1;
	myDecoded2 = -1;
	myBuffer = new char[BUFFER_SIZE];
}

ZLBase64InputStream::~ZLBase64InputStream() {
	delete[] myBuffer;
}

bool ZLBase64InputStream::open() {
	return myBaseStream->open();
}

int ZLBase64InputStream::read() {
	int result = myDecoded0;
	if (result != -1) {
		myDecoded0 = -1;
		return result;
	}
	result = myDecoded1;
	if (result != -1) {
		myDecoded1 = -1;
		return result;
	}
	result = myDecoded2;
	if (result != -1) {
		myDecoded2 = -1;
		return result;
	}
	fillDecodedBuffer();
	result = myDecoded0;
	myDecoded0 = -1;
	return result;
}

size_t ZLBase64InputStream::read(char *buffer, size_t maxSize) {
	if (maxSize == 0) {
		return 0;
	}
	size_t offset = 0;
	size_t ready = 0;
	if (myDecoded0 != -1) {
		buffer[offset] = (char)myDecoded0;
		myDecoded0 = -1;
		if (maxSize == 1) { return 1; }
		buffer[offset + 1] = (char)myDecoded1;
		myDecoded1 = -1;
		if (maxSize == 2) { return 2; }
		buffer[offset + 2] = (char)myDecoded2;
		myDecoded2 = -1;
		ready = 3;
	} else if (myDecoded1 != -1) {
		buffer[offset] = (char)myDecoded1;
		myDecoded1 = -1;
		if (maxSize == 1) { return 1; }
		buffer[offset + 1] = (char)myDecoded2;
		myDecoded2 = -1;
		ready = 2;
	} else if (myDecoded2 != -1) {
		buffer[offset] = (char)myDecoded2;
		myDecoded2 = -1;
		ready = 1;
	}
	for (; ready < maxSize - 2; ready += 3) {
		int first = -1;
		int second = -1;
		int third = -1;
		int fourth = -1;

		bool breaking = false;
		while (myBufferLength >= 0 && !breaking) {
			while (myBufferLength-- > 0) {
				const int digit = decode(myBuffer[myBufferOffset++]);
				if (digit != -1) {
					if (first == -1) {
						first = digit;
					} else if (second == -1) {
						second = digit;
					} else if (third == -1) {
						third = digit;
					} else {
						fourth = digit;
						breaking = true;
						break;
					}
				}
			}
			if (!breaking) {
				if (!fillBuffer()) {
					break;
				}
			}
		}
		if (first == -1) {
			return ready;// > 0 ? ready : -1;
		}
		buffer[offset + ready]     = (char)((first << 2) | (second >> 4));
		buffer[offset + ready + 1] = (char)((second << 4) | (third >> 2));
		buffer[offset + ready + 2] = (char)((third << 6) | fourth);
	}
	fillDecodedBuffer();
	for (; ready < maxSize; ++ready) {
		const int num = read();
		if (num == -1) {
			return ready;// > 0 ? ready : -1;
		}
		buffer[offset + ready] = (char)num;
	}
	return maxSize;
}

void ZLBase64InputStream::close() {
	return myBaseStream->close();
}

void ZLBase64InputStream::seek(int offset, bool absoluteOffset) {
	myBaseStream->seek(offset, absoluteOffset);
}

size_t ZLBase64InputStream::sizeOfOpened() {
	return myBaseStream->sizeOfOpened();
}

void ZLBase64InputStream::fillDecodedBuffer() {
	int first = -1;
	int second = -1;
	int third = -1;
	int fourth = -1;

	bool breaking = false;
	while (myBufferLength >= 0 && !breaking) {
		while (myBufferLength-- > 0) {
			const int digit = decode(myBuffer[myBufferOffset++]);
			if (digit != -1) {
				if (first == -1) {
					first = digit;
				} else if (second == -1) {
					second = digit;
				} else if (third == -1) {
					third = digit;
				} else {
					fourth = digit;
					breaking = true;
					break;
				}
			}
		}
		if (!breaking) {
			if (!fillBuffer()) {
				break;
			}
		}
	}
	if (first != -1) {
		myDecoded0 = (first << 2) | (second >> 4);
		myDecoded1 = 0xFF & ((second << 4) | (third >> 2));
		myDecoded2 = 0xFF & ((third << 6) | fourth);
	}
}

bool ZLBase64InputStream::fillBuffer() {
	myBufferLength = myBaseStream->read(myBuffer, BUFFER_SIZE);
	myBufferOffset = 0;
	if (myBufferLength == 0) {
		return false;
	}
	return true;
}

int ZLBase64InputStream::decode(char b) {
	switch (b) {
		default:
			return -1;
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
		case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
		case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
		case 'Y': case 'Z':
			return b - 'A';
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
		case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
		case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
		case 's': case 't': case 'u': case 'v': case 'w': case 'x':
		case 'y': case 'z':
			return b - 'a' + 26;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			return b - '0' + 52;
		case '+':
			return 62;
		case '/':
			return 63;
		case '=':
			return 64;
	}
}

size_t ZLBase64InputStream::offset() const {
	return myBaseStream->offset();
}
