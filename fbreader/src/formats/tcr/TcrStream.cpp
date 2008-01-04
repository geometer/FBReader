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

#include <string.h>

#include <algorithm>

#include <ZLFile.h>
#include <ZLZDecompressor.h>

#include "TcrStream.h"

TcrStream::TcrStream(ZLFile &file) : myBase(file.inputStream()) {
}

TcrStream::~TcrStream() {
	close();
}

bool TcrStream::open() {
	close();
	if (myBase.isNull() || !myBase->open()) {
		return false;
	}

	char header[9];
	if ((myBase->read(header, 9) != 9) || (strncmp(header, "!!8-Bit!!", 9) != 0)) {
		myBase->close();
		return false;
	}

	unsigned char entryLength;
	char entryBuffer[255];
	for (int i = 0; i < 256; ++i) {
		if ((myBase->read((char*)&entryLength, 1) != 1) ||
				((entryLength > 0) && (myBase->read(entryBuffer, entryLength) != entryLength))) {
			myBase->close();
			return false;
		}
		if (entryLength > 0) {
			myDictionary[i].append(entryBuffer, entryLength);
		}
	}

	return true;
}

void TcrStream::close() {
	if (!myBase.isNull()) {
		myBase->close();
	}
	for (int i = 0; i < 256; ++i) {
		myDictionary[i].erase();
	}
	myBuffer.erase();
}

size_t TcrStream::read(char *buffer, size_t maxSize) {
	size_t size = 0;
	if (myBuffer.length() > 0) {
		size += std::min(maxSize, myBuffer.length());
		if (buffer != 0) {
			strncpy(buffer, myBuffer.data(), size);
		}
		myBuffer.erase(0, size);
	}
	while (size < maxSize) {
		unsigned char index;
		if (myBase->read((char*)&index, 1) != 1) {
			break;
		}
		size_t len = myDictionary[index].length();
		if (len > 0) {
			size_t freeSize = maxSize - size;
			if (buffer != 0) {
				strncpy(buffer + size, myDictionary[index].data(), std::min(len, freeSize));
			}
			size += std::min(len, freeSize);
			if (len > freeSize) {
				myBuffer = myDictionary[index].substr(freeSize);
			}
		}
	}
	myOffset += size;
	return size;
}

void TcrStream::seek(int offset, bool absoluteOffset) {
	if (absoluteOffset) {
		offset -= this->offset();
	}
	if (offset > 0) {
		read(0, offset);
	} else if (offset < 0) {
		offset += this->offset();
		open();
		if (offset >= 0) {
			read(0, offset);
		}
	}
}

size_t TcrStream::offset() const {
	return myOffset;
}

size_t TcrStream::sizeOfOpened() {
	// TODO: implement
	return 0;
}
