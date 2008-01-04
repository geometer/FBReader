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

#include <ZLFile.h>

#include "PalmDocStream.h"
#include "DocDecompressor.h"

PalmDocStream::PalmDocStream(ZLFile &file) : PdbStream(file) {
}

PalmDocStream::~PalmDocStream() {
	close();
}

bool PalmDocStream::open() {
	if (!PdbStream::open()) {
		return false;
	}

	unsigned short version;
	PdbUtil::readUnsignedShort(*myBase, version);
	myIsCompressed = (version == 2);
	myBase->seek(6, false);
	unsigned short records;
	PdbUtil::readUnsignedShort(*myBase, records);
	myMaxRecordIndex = std::min(records, (unsigned short)(myHeader.Offsets.size() - 1));
	PdbUtil::readUnsignedShort(*myBase, myMaxRecordSize);
	if (myMaxRecordSize == 0) {
		return false;
	}
	myBuffer = new char[myMaxRecordSize];

	myRecordIndex = 0;

	return true;
}

bool PalmDocStream::fillBuffer() {
	while (myBufferOffset == myBufferLength) {
		if (myRecordIndex + 1 > myMaxRecordIndex) {
			return false;
		}
		++myRecordIndex;
		size_t currentOffset = myHeader.Offsets[myRecordIndex];
		if (currentOffset < myBase->offset()) {
			return false;
		}
		myBase->seek(currentOffset, true);
		size_t nextOffset =
			(myRecordIndex + 1 < myHeader.Offsets.size()) ?
				myHeader.Offsets[myRecordIndex + 1] : myBase->sizeOfOpened();
		if (nextOffset < currentOffset) {
			return false;
		}
		if (myIsCompressed) {
			myBufferLength = DocDecompressor().decompress(*myBase, myBuffer, nextOffset - currentOffset, myMaxRecordSize);
		} else {
			myBase->read(myBuffer, nextOffset - currentOffset);
			myBufferLength = nextOffset - currentOffset;
		}
		myBufferOffset = 0;
	}
	return true;
}
