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
#include <abstract/ZLZDecompressor.h>

#include "ZTXTStream.h"
#include "PdbReader.h"
#include "DocDecompressor.h"

ZTXTStream::ZTXTStream(ZLFile &file) : myBase(file.inputStream()) {
	myBuffer = 0;
}

ZTXTStream::~ZTXTStream() {
	close();
}

bool ZTXTStream::open() {
	close();
	if (myBase.isNull() || !myBase->open() || !myHeader.read(myBase)) {
		return false;
	}

	myBase->seek(myHeader.Offsets[0] - myBase->offset());

	myBase->seek(2);
	unsigned short records;
	PdbUtil::readUnsignedShort(myBase, records);
	myMaxRecordIndex = std::min(records, (unsigned short)(myHeader.Offsets.size() - 1));
	myBase->seek(4);
	PdbUtil::readUnsignedShort(myBase, myMaxRecordSize);
	if (myMaxRecordSize == 0) {
		return false;
	}
	myBuffer = new char[myMaxRecordSize];

	myRecordIndex = 0;
	myBufferLength = 0;
	myBufferOffset = 0;

	myOffset = 0;

	return true;
}

bool ZTXTStream::fillBuffer() {
	while (myBufferOffset == myBufferLength) {
		if (myRecordIndex + 1 > myMaxRecordIndex) {
			return false;
		}
		myRecordIndex++;
		size_t currentOffset = myHeader.Offsets[myRecordIndex];
		// Hmm, this works on examples from manybooks.net,
		// but I don't what thi code means :((
		if (myRecordIndex == 1) {
			currentOffset += 2;
		}
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
		myBufferLength = ZLZDecompressor(nextOffset - currentOffset).decompress(*myBase, myBuffer, myMaxRecordSize);
		myBufferOffset = 0;
	}
	return true;
}

size_t ZTXTStream::read(char *buffer, size_t maxSize) {
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

void ZTXTStream::close() {
	if (!myBase.isNull()) {
		myBase->close();
	}
	if (myBuffer != 0) {
		delete[] myBuffer;
		myBuffer = 0;
	}
}

void ZTXTStream::seek(size_t offset) {
	read(0, offset);
}

size_t ZTXTStream::offset() const {
	return myOffset;
}

size_t ZTXTStream::sizeOfOpened() {
	// TODO: implement
	return 0;
}
