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

#include <algorithm>

#include "ZLZip.h"
#include "ZLZipHeader.h"
#include "ZLZDecompressor.h"
#include "../ZLFSManager.h"

ZLZipInputStream::ZLZipInputStream(shared_ptr<ZLInputStream> &base, const std::string &name) : myBaseStream(base), myCompressedFileName(name), myUncompressedSize(0) {
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
				myBaseStream->seek(header.ExtraLength, false);
				break;
			}
		} else {
			myBaseStream->seek(header.NameLength, false);
		}
		ZLZipHeader::skipEntry(*myBaseStream, header);
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
	if (myAvailableSize == 0) {
		myAvailableSize = (size_t)-1;
	}

	if (myIsDeflated) {
		myDecompressor = new ZLZDecompressor(myAvailableSize);
	}

	myOffset = 0;
	return true;
}

size_t ZLZipInputStream::read(char *buffer, size_t maxSize) {
	if (myIsDeflated) {
		size_t realSize = myDecompressor->decompress(*myBaseStream, buffer, maxSize);
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
	myDecompressor = 0;
	myBaseStream->close();
}

void ZLZipInputStream::seek(int offset, bool absoluteOffset) {
	if (absoluteOffset) {
		offset -= this->offset();
	}
	if (offset > 0) {
		read(0, offset);
	} else if (offset < 0) {
		offset += this->offset();
		open();
		if (offset >= 0) {
			read(0, offset);
		}
	}
}

size_t ZLZipInputStream::offset() const {
	return myOffset;
}

size_t ZLZipInputStream::sizeOfOpened() {
	// TODO: implement for files with Flags & 0x08
	return myUncompressedSize;
}
