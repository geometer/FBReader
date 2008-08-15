/*
 * Copyright (C) 2008 Geometer Plus <contact@geometerplus.com>
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

#include "ZLZip.h"
#include "ZLZipHeader.h"

ZLZipCache ZLZipCache::Instance;

ZLZipCache::Info::Info() : Offset(-1) {
}

void ZLZipCache::addToCache(const ZLFile &file) {
	if (contains(file.path())) {
		return;
	}

	std::map<std::string,Info> &infoMap = myMap[file.path()];

	shared_ptr<ZLInputStream> stream = file.inputStream();

	if (stream.isNull() || !stream->open()) {
		return;
	}

	ZLZipHeader header;
	while (header.readFrom(*stream)) {
		std::string entryName(header.NameLength, '\0');
		if ((unsigned int)stream->read((char*)entryName.data(), header.NameLength) == header.NameLength) {
			Info &info = infoMap[entryName];
			info.Offset = stream->offset() + header.ExtraLength;
			info.CompressionMethod = header.CompressionMethod;
			info.CompressedSize = header.CompressedSize;
			info.UncompressedSize = header.UncompressedSize;
		}
		ZLZipHeader::skipEntry(*stream, header);
	}
	stream->close();
}

bool ZLZipCache::contains(const std::string &fileName) const {
	return myMap.find(fileName) != myMap.end();
}

ZLZipCache::Info ZLZipCache::info(const std::string &fileName, const std::string &entryName) const {
	std::map<std::string,std::map<std::string,Info> >::const_iterator it = myMap.find(fileName);
	if (it == myMap.end()) {
		return Info();
	}

	std::map<std::string,Info>::const_iterator jt = it->second.find(entryName);
	return (jt != it->second.end()) ? jt->second : Info();
}
