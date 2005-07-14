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
#include "ZLZipHeader.h"
#include "../ZLFSManager.h"

const size_t IN_BUFFER_SIZE = 2048;
const size_t OUT_BUFFER_SIZE = 32768;

ZLZipInputStream::ZLZipInputStream(shared_ptr<ZLInputStream> &base, const std::string &name) : myBaseStream(base), myCompressedFileName(name) {
	myZStream = 0;
	myInBuffer = 0;
	myOutBuffer = 0;
	myUncompressedSize = 0;
}

ZLZipInputStream::~ZLZipInputStream() {
	close();
}

bool ZLZipInputStream::open() {
	close();

	if (!myBaseStream->open()) {
		return false;
	}

	ZLZipHeader header;
	while (true) {
		if (!header.readFrom(*myBaseStream)) {
			close();
			return false;
		}
		if (header.NameLength == myCompressedFileName.length()) {
			char *buffer = new char[header.NameLength];
			myBaseStream->read(buffer, header.NameLength);
			std::string str;
			str.append(buffer, header.NameLength);
			delete[] buffer;
			if (str == myCompressedFileName) {
				myBaseStream->seek(header.ExtraLength);
				break;
			}
		} else {
			myBaseStream->seek(header.NameLength);
		}
		myBaseStream->seek(header.ExtraLength + header.CompressedSize);
		if (header.Flags & 0x04) {
			myBaseStream->seek(12);
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
	myUncompressedSize = header.UncompressedSize;
	myAvailableSize = header.CompressedSize;

	if (myIsDeflated) {
		myZStream = new z_stream;
		memset(myZStream, 0, sizeof(z_stream));
		inflateInit2(myZStream, -MAX_WBITS);
	}

	myInBuffer = new char[IN_BUFFER_SIZE];
	myOutBuffer = new char[OUT_BUFFER_SIZE];
	myOffset = 0;
	return true;
}

size_t ZLZipInputStream::read(char *buffer, size_t maxSize) {
	if (myIsDeflated) {
		while ((myBuffer.length() < maxSize) && (myAvailableSize > 0)) {
			size_t size = std::min(myAvailableSize, (size_t)IN_BUFFER_SIZE);

			myZStream->next_in = (Bytef*)myInBuffer;
			myZStream->avail_in = myBaseStream->read(myInBuffer, size);
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
	} else {
		size_t realSize = std::min(maxSize, myAvailableSize);
		myAvailableSize -= realSize;
		myOffset += realSize;
		return myBaseStream->read(buffer, realSize);
	}
}

void ZLZipInputStream::close() {
	if (myInBuffer == 0) {
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

	myBaseStream->close();
}

void ZLZipInputStream::seek(size_t offset) {
	read(0, offset);
}

size_t ZLZipInputStream::offset() const {
	return myOffset;
}

size_t ZLZipInputStream::sizeOfOpened() {
	return myUncompressedSize;
}
