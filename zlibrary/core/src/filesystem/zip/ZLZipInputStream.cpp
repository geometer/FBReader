/*
 * Copyright (C) 2004-2009 Geometer Plus <contact@geometerplus.com>
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

class ZLZipEntryCache : public ZLUserData {

public:
	struct Info {
		Info();

		int Offset;
		int CompressionMethod;
		int CompressedSize;
		int UncompressedSize;
	};

public:
	ZLZipEntryCache(ZLInputStream &baseStream);
	Info info(const std::string &entryName) const;

private:
	std::map<std::string,Info> myInfoMap;
};

ZLZipEntryCache::Info::Info() : Offset(-1) {
}

ZLZipEntryCache::ZLZipEntryCache(ZLInputStream &baseStream) {
	if (!baseStream.open()) {
		return;
	}

	ZLZipHeader header;
	while (header.readFrom(baseStream)) {
		std::string entryName(header.NameLength, '\0');
		if ((unsigned int)baseStream.read((char*)entryName.data(), header.NameLength) == header.NameLength) {
			Info &info = myInfoMap[entryName];
			info.Offset = baseStream.offset() + header.ExtraLength;
			info.CompressionMethod = header.CompressionMethod;
			info.CompressedSize = header.CompressedSize;
			info.UncompressedSize = header.UncompressedSize;
		}
		ZLZipHeader::skipEntry(baseStream, header);
	}
	baseStream.close();
}

ZLZipEntryCache::Info ZLZipEntryCache::info(const std::string &entryName) const {
	std::map<std::string,Info>::const_iterator it = myInfoMap.find(entryName);
	return (it != myInfoMap.end()) ? it->second : Info();
}

ZLZipInputStream::ZLZipInputStream(shared_ptr<ZLInputStream> &base, const std::string &entryName) : myBaseStream(base), myEntryName(entryName), myUncompressedSize(0) {
}

ZLZipInputStream::~ZLZipInputStream() {
	close();
}

bool ZLZipInputStream::open() {
	close();

	static const std::string zipEntryMapKey = "zipEntryMap";
	shared_ptr<ZLUserData> data = myBaseStream->getUserData(zipEntryMapKey);
	if (data.isNull()) {
		data = new ZLZipEntryCache(*myBaseStream);
		myBaseStream->addUserData(zipEntryMapKey, data);
	}

	if (!myBaseStream->open()) {
		return false;
	}

	ZLZipEntryCache::Info info = ((const ZLZipEntryCache&)*data).info(myEntryName);
	if (info.Offset == -1) {
		close();
		return false;
	}
	myBaseStream->seek(info.Offset, true);
	myBaseOffset = myBaseStream->offset();

	if (info.CompressionMethod == 0) {
		myIsDeflated = false;
	} else if (info.CompressionMethod == 8) {
		myIsDeflated = true;
	} else {
		close();
		return false;
	}
	myUncompressedSize = info.UncompressedSize;
	myAvailableSize = info.CompressedSize;
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
	size_t realSize = 0;
	myBaseStream->seek(myBaseOffset, true);
	if (myIsDeflated) {
		realSize = myDecompressor->decompress(*myBaseStream, buffer, maxSize);
		myOffset += realSize;
	} else {
		realSize = myBaseStream->read(buffer, std::min(maxSize, myAvailableSize));
		myAvailableSize -= realSize;
		myOffset += realSize;
	}
	myBaseOffset = myBaseStream->offset();
	return realSize;
}

void ZLZipInputStream::close() {
	myDecompressor = 0;
	if (!myBaseStream.isNull()) {
		myBaseStream->close();
	}
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
