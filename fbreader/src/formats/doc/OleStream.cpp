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

#include "OleStream.h"
#include "NumUtil.h"

static const int FLAG_DOT = 0x0001;
static const int FLAG_GLSY = 0x0002;
static const int FLAG_COMPLEX = 0x0004;
static const int FLAG_PICTURES = 0x0008;
static const int FLAG_ENCRYPTED = 0x100;
static const int FLAG_READONLY = 0x400;
static const int FLAG_RESERVED = 0x800;
static const int FLAG_EXTCHAR = 0x1000;

static const int FLAGS_OFFSET = 0xA;

static const int FILEINFO_BLOCK_VERSION = 0x2;
static const int PRODUCT_VERSION = 0x4;
static const int LANGUAGE = 0x6;

static const int CHARSET = 0x14;
static const int DEFAULT_CHARSET = 0x100;

static const int START_OF_TEXT = 0x18;
static const int END_OF_TEXT = 0x1c;

OleStream::OleStream(shared_ptr<OleStorage> storage, OleEntry oleEntry, shared_ptr<ZLInputStream> stream) :
	myStorage(storage), myOleEntry(oleEntry), myInputStream(stream)  {

	myCurBlock = 0;
	myOleOffset = 0;
	myFileOffset = 0;
}


bool OleStream::open() {
	if (myOleEntry.type != OleEntry::STREAM) {
		return false;
	}

	static const size_t HEADER_SIZE = 128;

	char header[HEADER_SIZE];
	long offset = read(header, 1, HEADER_SIZE);

	int flags = NumUtil::getUInt16(header, FLAGS_OFFSET);

	if (flags & FLAG_COMPLEX) {
		ZLLogger::Instance().println("OleStream","This was fast-saved. Some information is lost");
		//lost_info = (flags & 0xF0)>>4);
	}

	if (flags & FLAG_EXTCHAR) {
		ZLLogger::Instance().println("OleStream","File uses extended character set (get_word8_char)");
	} else {
		ZLLogger::Instance().println("OleStream","File uses get_8bit_char character set");
	}

	if (flags & FLAG_ENCRYPTED) {
		ZLLogger::Instance().println("OleStream","File is encrypted");
		// Encryption key = %08lx ; NumUtil::getlong(header,14)
		return false;
	}

	unsigned int charset = NumUtil::getUInt16(header,CHARSET);
	if (charset&&charset !=DEFAULT_CHARSET) {
		ZLLogger::Instance().println("OleStream", "Using not default character set %d");
	} else {
		ZLLogger::Instance().println("OleStream","Using default character set");
	}

	long startOfText = NumUtil::getInt32(header,START_OF_TEXT);
	long endOfText = NumUtil::getInt32(header,END_OF_TEXT);
	myLength = endOfText - startOfText;
	startOfText -= offset;

	//moving to start of text
	char buf[2];
	for (long i = 0; i < startOfText; ++i) {
		read(buf, 1, 1);
		if (eof()) {
			ZLLogger::Instance().println("OleStream","File ended before textstart. Probably it is broken.");
			return false;
		}
	}
	return true;
}

size_t OleStream::read(void *ptr, size_t size, size_t nmemb) {
	long int length = size*nmemb, readedBytes=0;
	long int blockNumber, modBlock, toReadBlocks, toReadBytes, bytesInBlock;
	long int sectorSize;
	long int newOffset;
	char *cptr = (char*)ptr;
	if( myOleOffset+length > myOleEntry.length ) {
		length = myOleEntry.length - myOleOffset;
	}

	sectorSize = (myOleEntry.isBigBlock ? myStorage->getSectorSize() : myStorage->getShortSectorSize());
	blockNumber=myOleOffset/sectorSize;

	if ( blockNumber >= myOleEntry.blocks.size() || length <=0 ) {
		return 0;
	}

	modBlock = myOleOffset % sectorSize;
	bytesInBlock = sectorSize - modBlock;
	if(bytesInBlock < length) {
		toReadBlocks = (length - bytesInBlock) / sectorSize;
		toReadBytes  = (length - bytesInBlock) % sectorSize;
	} else {
		toReadBlocks = toReadBytes = 0;
	}

	newOffset = myStorage->calculateBlockOffset(myOleEntry,blockNumber) + modBlock;
	if (myFileOffset != newOffset) {
		myFileOffset=newOffset;
		myInputStream->seek(myFileOffset, true);
	}
	readedBytes = myInputStream->read(cptr, std::min(length,bytesInBlock));
	myFileOffset += readedBytes;
	for(long int i = 0; i < toReadBlocks; ++i) {
		int readbytes;
		blockNumber++;
		newOffset = myStorage->calculateBlockOffset(myOleEntry,blockNumber);
		if (newOffset != myFileOffset); //TODO strange thing; FIX IT
		myInputStream->seek(myFileOffset=newOffset, true);
		readbytes = myInputStream->read(cptr+readedBytes, std::min(length - readedBytes, sectorSize));
		readedBytes +=readbytes;
		myFileOffset +=readbytes;
	}
	if(toReadBytes > 0) {
		int readbytes;
		blockNumber++;
		newOffset = myStorage->calculateBlockOffset(myOleEntry,blockNumber);
		myInputStream->seek(myFileOffset=newOffset, true);
		readbytes = myInputStream->read(cptr+readedBytes, toReadBytes);
		readedBytes +=readbytes;
		myFileOffset +=readbytes;
	}
	myOleOffset += readedBytes;
	return readedBytes;
}

bool OleStream::eof() {
	return (myOleOffset >= myOleEntry.length); //TODO fix comparing signed/unsigned
}


bool OleStream::close() {
	return true;
}

int OleStream::seek(long offset, bool absoluteOffset) {
	long int newOleOffset=0;
	long int newFileOffset;
	int sectorSize, modBlock, blockNumber;

	if (absoluteOffset) {
		newOleOffset = offset;
	} else {
		newOleOffset = myOleOffset + offset;
	}

	if(newOleOffset < 0) {
		newOleOffset = 0;
	}

	newOleOffset = std::min((unsigned long int)newOleOffset, myOleEntry.length);

	sectorSize = (myOleEntry.isBigBlock ? myStorage->getSectorSize() : myStorage->getShortSectorSize());
	blockNumber = newOleOffset / sectorSize;
	if ( blockNumber >= myOleEntry.blocks.size() ) {
		return -1;
	}

	modBlock = newOleOffset % sectorSize;
	newFileOffset = myStorage->calculateBlockOffset(myOleEntry,blockNumber) + modBlock;
	myInputStream->seek(myFileOffset = newFileOffset, true);
	myOleOffset = newOleOffset;

	return 0;
}

long OleStream::tell() {
	return myOleOffset;
}
