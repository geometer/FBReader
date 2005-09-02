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

#include "PdbStream.h"

PdbStream::PdbStream(ZLFile &file) : myBase(file.inputStream()) {
	myBuffer = 0;
}

PdbStream::~PdbStream() {
}

size_t PdbStream::read(char *buffer, size_t maxSize) {
	size_t realSize = 0;
	while (realSize < maxSize) {
		if (!fillBuffer()) {
			break;
		}
		size_t size = std::min((size_t)(maxSize - realSize), (size_t)(myBufferLength - myBufferOffset));
		if (size > 0) {
			if (buffer != 0) {
				memcpy(buffer + realSize, myBuffer + myBufferOffset, size);
			}
			realSize += size;
			myBufferOffset += size;
		}
	}
			
	myOffset += realSize;
	return realSize;
}

void PdbStream::close() {
	if (!myBase.isNull()) {
		myBase->close();
	}
	if (myBuffer != 0) {
		delete[] myBuffer;
		myBuffer = 0;
	}
}

void PdbStream::seek(size_t offset) {
	read(0, offset);
}

size_t PdbStream::offset() const {
	return myOffset;
}

size_t PdbStream::sizeOfOpened() {
	// TODO: implement
	return 0;
}
