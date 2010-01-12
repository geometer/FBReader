/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#include <ZLFile.h>
#include <ZLZDecompressor.h>

#include "ZTXTStream.h"

ZTXTStream::ZTXTStream(ZLFile &file) : PdbStream(file) {
}

ZTXTStream::~ZTXTStream() {
	close();
}

bool ZTXTStream::open() {
	if (!PdbStream::open()) {
		return false;
	}

	myBase->seek(2, false);
	unsigned short recordNumber;
	PdbUtil::readUnsignedShort(*myBase, recordNumber);
	myMaxRecordIndex = std::min(recordNumber, (unsigned short)(header().Offsets.size() - 1));
	myBase->seek(4, false);
	PdbUtil::readUnsignedShort(*myBase, myMaxRecordSize);
	if (myMaxRecordSize == 0) {
		return false;
	}
	myBuffer = new char[myMaxRecordSize];

	myRecordIndex = 0;

	return true;
}

bool ZTXTStream::fillBuffer() {
	while (myBufferOffset == myBufferLength) {
		if (myRecordIndex + 1 > myMaxRecordIndex) {
			return false;
		}
		++myRecordIndex;
		size_t currentOffset = recordOffset(myRecordIndex);
		// Hmm, this works on examples from manybooks.net,
		// but I don't understand what this code means :((
		if (myRecordIndex == 1) {
			currentOffset += 2;
		}
		if (currentOffset < myBase->offset()) {
			return false;
		}
		myBase->seek(currentOffset, true);
		const size_t nextOffset = recordOffset(myRecordIndex + 1);
		if (nextOffset < currentOffset) {
			return false;
		}
		myBufferLength = ZLZDecompressor(nextOffset - currentOffset).decompress(*myBase, myBuffer, myMaxRecordSize);
		myBufferOffset = 0;
	}
	return true;
}
