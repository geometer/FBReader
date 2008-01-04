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

#include <ZLFile.h>
#include <ZLZDecompressor.h>

#include "PluckerTextStream.h"
#include "PdbReader.h"
#include "DocDecompressor.h"

PluckerTextStream::PluckerTextStream(ZLFile &file) : PdbStream(file) {
	myFullBuffer = 0;
}

PluckerTextStream::~PluckerTextStream() {
	close();
}

bool PluckerTextStream::open() {
	if (!PdbStream::open()) {
		return false;
	}

	PdbUtil::readUnsignedShort(*myBase, myCompressionVersion);

	myBuffer = new char[65536];
	myFullBuffer = new char[65536];

	myRecordIndex = 0;

	return true;
}

bool PluckerTextStream::fillBuffer() {
	while (myBufferOffset == myBufferLength) {
		if (myRecordIndex + 1 > myHeader.Offsets.size() - 1) {
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
		processRecord(nextOffset - currentOffset);
	}
	return true;
}

void PluckerTextStream::close() {
	if (myFullBuffer != 0) {
		delete[] myFullBuffer;
		myFullBuffer = 0;
	}
	PdbStream::close();
}

void PluckerTextStream::processRecord(size_t recordSize) {
	myBase->seek(2, false);

	unsigned short paragraphs;
	PdbUtil::readUnsignedShort(*myBase, paragraphs);

	unsigned short size;
	PdbUtil::readUnsignedShort(*myBase, size);

	unsigned char type;
	myBase->read((char*)&type, 1);
	if (type > 1) { // this record is not text record
		return;
	}

	myBase->seek(1, false);

	std::vector<int> pars;
	for (int i = 0; i < paragraphs; ++i) {
		unsigned short pSize;
		PdbUtil::readUnsignedShort(*myBase, pSize);
		pars.push_back(pSize);
		myBase->seek(2, false);
	}

	bool doProcess = false;
	if (type == 0) {
		doProcess = myBase->read(myFullBuffer, size) == size;
	} else if (myCompressionVersion == 1) {
		doProcess =
			DocDecompressor().decompress(*myBase, myFullBuffer, recordSize - 8 - 4 * paragraphs, size) == size;
	} else if (myCompressionVersion == 2) {
		myBase->seek(2, false);
		doProcess =
			ZLZDecompressor(recordSize - 10 - 4 * paragraphs).decompress(*myBase, myFullBuffer, size) == size;
	}
	if (doProcess) {
		myBufferLength = 0;
		myBufferOffset = 0;

		char *start = myFullBuffer;
		char *end = myFullBuffer;

		for (std::vector<int>::const_iterator it = pars.begin(); it != pars.end(); ++it) {
			start = end;
			end = start + *it;
			if (end > myFullBuffer + size) {
				break;
			}
			processTextParagraph(start, end);
		}
	}
}

void PluckerTextStream::processTextParagraph(char *start, char *end) {
	char *textStart = start;
	bool functionFlag = false;
	for (char *ptr = start; ptr < end; ++ptr) {
		if (*ptr == 0) {
			functionFlag = true;
			if (ptr != textStart) {
				memcpy(myBuffer + myBufferLength, textStart, ptr - textStart);
				myBufferLength += ptr - textStart;
			}
		} else if (functionFlag) {
			int paramCounter = ((unsigned char)*ptr) % 8;
			if (end - ptr > paramCounter + 1) {
				ptr += paramCounter;
			} else {
				ptr = end - 1;
			}
			functionFlag = false;
			textStart = ptr + 1;
		}
	}
	if (end != textStart) {
		memcpy(myBuffer + myBufferLength, textStart, end - textStart);
		myBufferLength += end - textStart;
	}
}
