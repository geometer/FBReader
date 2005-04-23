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

#include "ZLZipInputStream.h"
#include "ZipHeader.h"
#include "ZLFSManager.h"
#include "ZLInputStream.h"

const unsigned int IN_BUFFER_SIZE = 2048;
const unsigned int OUT_BUFFER_SIZE = 32768;

ZLZipInputStream::ZLZipInputStream(const std::string &name) {
	int index = name.find(':');
	myFileStream = ZLFSManager::instance().createInputStream(name.substr(0, index));
	myCompressedFileName = name.substr(index + 1);
	myZStream = 0;
	myInBuffer = new char[IN_BUFFER_SIZE];
	myOutBuffer = new char[OUT_BUFFER_SIZE];
}

ZLZipInputStream::~ZLZipInputStream() {
	close();
	delete myFileStream;
	delete[] myInBuffer;
	delete[] myOutBuffer;
}

bool ZLZipInputStream::open() {
	close();

	if (!myFileStream->open()) {
		return false;
	}

	ZipHeader header;
	while (true) {
		if (!header.readFrom(*myFileStream)) {
			close();
			return false;
		}
		if (header.NameLength == myCompressedFileName.length()) {
			char *buffer = new char[header.NameLength];
			myFileStream->read(buffer, header.NameLength);
			std::string str;
			str.append(buffer, header.NameLength);
			delete[] buffer;
			if (str == myCompressedFileName) {
				myFileStream->seek(header.ExtraLength);
				break;
			}
		} else {
			myFileStream->seek(header.NameLength);
		}
		myFileStream->seek(header.ExtraLength + header.CompressedSize);
		if (header.Flags & 0x04) {
			myFileStream->seek(12);
		}
	}
	if (header.CompressionMethod == 0) {
		myIsDeflated = false;
	} else if (header.CompressionMethod == 8) {
		myIsDeflated = true;
	} else {
		close();
		return false;
	}
	myAvailableSize = header.CompressedSize;

	if (myIsDeflated) {
		myZStream = new z_stream;
		memset(myZStream, 0, sizeof(z_stream));
		inflateInit2(myZStream, -MAX_WBITS);
	}

	return true;
}

int ZLZipInputStream::read(char *buffer, int maxSize) {
	if (myIsDeflated) {
		while (((int)myBuffer.length() < maxSize) && (myAvailableSize > 0)) {
			unsigned int size = (myAvailableSize < IN_BUFFER_SIZE) ? myAvailableSize : IN_BUFFER_SIZE;

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
				myBuffer.append(myOutBuffer, OUT_BUFFER_SIZE - myZStream->avail_out);
			}
		}

		if (maxSize > (int)myBuffer.length()) {
			maxSize = myBuffer.length();
		}
		strncpy(buffer, myBuffer.data(), maxSize);
		myBuffer.erase(0, maxSize);
		return maxSize;
	} else {
		if ((int)myAvailableSize < maxSize) {
			maxSize = myAvailableSize;
		}
		myAvailableSize -= maxSize;
		return myFileStream->read(buffer, maxSize);
	}
}

int ZLZipInputStream::read(unsigned short *p) {
	return read((char*)p, 2);
}

int ZLZipInputStream::read(unsigned long *p) {
	return read((char*)p, 4);
}

void ZLZipInputStream::close() {
	if (myZStream != 0) {
		inflateEnd(myZStream);
		delete myZStream;
		myZStream = 0;
	}

	myBuffer.erase();

	myFileStream->close();
}

void ZLZipInputStream::seek(int offset) {
	// TODO: implement
}

int ZLZipInputStream::offset() const {
	// TODO: implement
	return 0;
}
