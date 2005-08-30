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

#include "PalmDocStream.h"
#include "PdbReader.h"
#include "DocDecompressor.h"

PalmDocStream::PalmDocStream(ZLFile &file) : myBase(file.inputStream()) {
	myBuffer = 0;
}

PalmDocStream::~PalmDocStream() {
	close();
}

bool PalmDocStream::open() {
	close();
	if (myBase.isNull() || !myBase->open() || !myHeader.read(myBase)) {
		return false;
	}

	myBase->seek(myHeader.Offsets[0] - myBase->offset());

	unsigned short version;
	PdbUtil::readUnsignedShort(myBase, version);
	myIsCompressed = (version == 2);
	myBase->seek(6);
	unsigned short records;
	PdbUtil::readUnsignedShort(myBase, records);
	myMaxRecordIndex = std::min(records, (unsigned short)(myHeader.Offsets.size() - 1));
	unsigned short recordSize;
	PdbUtil::readUnsignedShort(myBase, recordSize);
	myBuffer = new char[recordSize];

	myRecordIndex = 0;
	myBufferLength = 0;
	myBufferOffset = 0;

	myOffset = 0;
	return true;
}

bool PalmDocStream::fillBuffer() {
	while (myBufferOffset == myBufferLength) {
		if (myRecordIndex + 1 > myMaxRecordIndex) {
			return false;
		}
		myRecordIndex++;
		size_t currentOffset = myHeader.Offsets[myRecordIndex];
		if (currentOffset < myBase->offset()) {
			return false;
		}
		myBase->seek(currentOffset - myBase->offset());
		size_t nextOffset =
			(myRecordIndex + 1 < myHeader.Offsets.size()) ?
				myHeader.Offsets[myRecordIndex + 1] : myBase->sizeOfOpened();
		if (nextOffset < currentOffset) {
			return false;
		}
		if (myIsCompressed) {
			myBufferLength = DocDecompressor().decompress(*myBase, myBuffer, nextOffset - currentOffset);
		} else {
			myBase->read(myBuffer, nextOffset - currentOffset);
			myBufferLength = nextOffset - currentOffset;
		}
		myBufferOffset = 0;
	}
	return true;
}

size_t PalmDocStream::read(char *buffer, size_t maxSize) {
	size_t realSize = 0;
	while (realSize < maxSize) {
		if (!fillBuffer()) {
			break;
		}
		size_t size = std::min(maxSize - realSize, myBufferLength - myBufferOffset);
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

void PalmDocStream::close() {
	if (!myBase.isNull()) {
		myBase->close();
	}
	if (myBuffer != 0) {
		delete[] myBuffer;
		myBuffer = 0;
	}
}

void PalmDocStream::seek(size_t offset) {
	read(0, offset);
}

size_t PalmDocStream::offset() const {
	return myOffset;
}

size_t PalmDocStream::sizeOfOpened() {
	// TODO: implement
	return 0;
}
