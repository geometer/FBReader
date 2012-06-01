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

#include <ZLLogger.h>

#include <stdio.h>

#include "OleStream.h"
#include "OleUtil.h"

OleStream::OleStream(shared_ptr<OleStorage> storage, OleEntry oleEntry, shared_ptr<ZLInputStream> stream) :
	myStorage(storage),
	myOleEntry(oleEntry),
	myBaseStream(stream) {
	myCurBlock = 0;
	myOleOffset = 0;
	//myFileOffset = 0;
}


bool OleStream::open() {
	if (myOleEntry.type != OleEntry::STREAM) {
		return false;
	}
	return true;
}

size_t OleStream::read(char *buffer, size_t maxSize) {
	size_t length = maxSize;
	size_t readedBytes = 0;
	unsigned long newFileOffset;

	unsigned int curBlockNumber, modBlock, toReadBlocks, toReadBytes, bytesLeftInCurBlock;
	unsigned int sectorSize;

	if( myOleOffset + length > myOleEntry.length ) {
		length = myOleEntry.length - myOleOffset;
	}

	sectorSize = (myOleEntry.isBigBlock ? myStorage->getSectorSize() : myStorage->getShortSectorSize());

	curBlockNumber = myOleOffset / sectorSize;
	if (curBlockNumber >= myOleEntry.blocks.size()) {
		return 0;
	}
	modBlock = myOleOffset % sectorSize;
	bytesLeftInCurBlock = sectorSize - modBlock;
	if (bytesLeftInCurBlock < length) {
		toReadBlocks = (length - bytesLeftInCurBlock) / sectorSize;
		toReadBytes  = (length - bytesLeftInCurBlock) % sectorSize;
	} else {
		toReadBlocks = toReadBytes = 0;
	}

	newFileOffset = myStorage->calcFileOffsetByBlockNumber(myOleEntry, curBlockNumber) + modBlock;
	myBaseStream->seek(newFileOffset, true);

	readedBytes = myBaseStream->read(buffer, std::min(length, bytesLeftInCurBlock));
	for(unsigned long i = 0; i < toReadBlocks; ++i) {
		size_t readbytes;
		++curBlockNumber;
		newFileOffset = myStorage->calcFileOffsetByBlockNumber(myOleEntry, curBlockNumber);
		myBaseStream->seek(newFileOffset, true);
		readbytes = myBaseStream->read(buffer + readedBytes, std::min(length - readedBytes, sectorSize));
		readedBytes += readbytes;
	}
	if(toReadBytes > 0) {
		size_t readbytes;
		++curBlockNumber;
		newFileOffset = myStorage->calcFileOffsetByBlockNumber(myOleEntry, curBlockNumber);
		myBaseStream->seek(newFileOffset, true);
		readbytes = myBaseStream->read(buffer + readedBytes, toReadBytes);
		readedBytes += readbytes;
	}
	myOleOffset += readedBytes;
	return readedBytes;
}

bool OleStream::eof() const {
	return (myOleOffset >= myOleEntry.length);
}


void OleStream::close() {
}

bool OleStream::seek(unsigned long offset, bool absoluteOffset) {
	unsigned long newOleOffset = 0;
	unsigned long newFileOffset;
	unsigned int sectorSize, modBlock, blockNumber;

	if (absoluteOffset) {
		newOleOffset = offset;
	} else {
		newOleOffset = myOleOffset + offset;
	}

	newOleOffset = std::min(newOleOffset, myOleEntry.length);

	sectorSize = (myOleEntry.isBigBlock ? myStorage->getSectorSize() : myStorage->getShortSectorSize());
	blockNumber = newOleOffset / sectorSize;
	if (blockNumber >= myOleEntry.blocks.size()) {
		return false;
	}

	modBlock = newOleOffset % sectorSize;
	newFileOffset = myStorage->calcFileOffsetByBlockNumber(myOleEntry, blockNumber) + modBlock;
	myBaseStream->seek(newFileOffset, true);
	myOleOffset = newOleOffset;
	return true;
}

size_t OleStream::offset() {
	return myOleOffset;
}
