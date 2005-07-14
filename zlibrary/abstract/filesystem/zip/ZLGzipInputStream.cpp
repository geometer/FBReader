/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <algorithm>

#include "ZLZip.h"

const size_t IN_BUFFER_SIZE = 2048;
const size_t OUT_BUFFER_SIZE = 32768;

ZLGzipInputStream::ZLGzipInputStream(shared_ptr<ZLInputStream> stream) : myFileStream(stream) {
	myFileSize = 0;
	myZStream = 0;
	myInBuffer = 0;
	myOutBuffer = 0;
}

ZLGzipInputStream::~ZLGzipInputStream() {
	close();
}

bool ZLGzipInputStream::open() {
	close();

	if (!myFileStream->open()) {
		return false;
	}

	myFileSize = myFileStream->sizeOfOpened();

	unsigned char id1;
	unsigned char id2;
	unsigned char cm;

	myFileStream->read((char*)&id1, 1);
	myFileStream->read((char*)&id2, 1);
	myFileStream->read((char*)&cm, 1);
	if ((id1 != 31) || (id2 != 139) || (cm != 8)) {
		myFileStream->close();
		return false;
	}

	//const unsigned char FTEXT = 1 << 0;
	const unsigned char FHCRC = 1 << 1;
	const unsigned char FEXTRA = 1 << 2;
	const unsigned char FNAME = 1 << 3;
	const unsigned char FCOMMENT = 1 << 4;
	unsigned char flg;
	myFileStream->read((char*)&flg, 1);
	myFileStream->seek(6);
	if (flg & FEXTRA) {
		unsigned char b0, b1;
		myFileStream->read((char*)&b0, 1);
		myFileStream->read((char*)&b1, 1);
		unsigned short xlen = ((unsigned short)b1) << 8 + b0;
		myFileStream->seek(xlen);
	}
	if (flg & FNAME) {
		unsigned char b;
		do {
			myFileStream->read((char*)&b, 1);
		} while (b != 0);
	}
	if (flg & FCOMMENT) {
		unsigned char b;
		do {
			myFileStream->read((char*)&b, 1);
		} while (b != 0);
	}
	if (flg & FHCRC) {
		myFileStream->seek(2);
	}

	myAvailableSize = myFileSize - myFileStream->offset() - 8;

	myZStream = new z_stream;
	memset(myZStream, 0, sizeof(z_stream));
	inflateInit2(myZStream, -MAX_WBITS);

	myInBuffer = new char[IN_BUFFER_SIZE];
	myOutBuffer = new char[OUT_BUFFER_SIZE];
	myOffset = 0;

	return true;
}

size_t ZLGzipInputStream::read(char *buffer, size_t maxSize) {
	while ((myBuffer.length() < maxSize) && (myAvailableSize > 0)) {
		size_t size = std::min(myAvailableSize, (size_t)IN_BUFFER_SIZE);

		myZStream->next_in = (Bytef*)myInBuffer;
		myZStream->avail_in = myFileStream->read(myInBuffer, size);
		if (myZStream->avail_in == size) {
			myAvailableSize -= size;
		} else {
			myAvailableSize = 0;
		}
		while (myZStream->avail_in > 0) {
			myZStream->avail_out = OUT_BUFFER_SIZE;
			myZStream->next_out = (Bytef*)myOutBuffer;
			int code = inflate(myZStream, Z_SYNC_FLUSH);
			if ((code != Z_OK) && (code != Z_STREAM_END)) {
				break;
			}
			if (OUT_BUFFER_SIZE == myZStream->avail_out) {
				break;
			}
			myBuffer.append(myOutBuffer, OUT_BUFFER_SIZE - myZStream->avail_out);
		}
	}

	size_t realSize = std::min(maxSize, myBuffer.length());
	if (buffer != 0) {
		memcpy(buffer, myBuffer.data(), realSize);
	}
	myBuffer.erase(0, realSize);
	myOffset += realSize;
	return realSize;
}

void ZLGzipInputStream::close() {
	if (myInBuffer != 0) {
		delete[] myInBuffer;
		delete[] myOutBuffer;
		myInBuffer = 0;
		myOutBuffer = 0;
	}
	if (myZStream != 0) {
		inflateEnd(myZStream);
		delete myZStream;
		myZStream = 0;
	}

	myBuffer.erase();

	myFileStream->close();
}

void ZLGzipInputStream::seek(size_t offset) {
	read(0, offset);
}

size_t ZLGzipInputStream::offset() const {
	return myOffset;
}

size_t ZLGzipInputStream::sizeOfOpened() {
	// TODO: implement
	return 0;
}
