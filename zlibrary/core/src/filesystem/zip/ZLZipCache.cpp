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

void ZLZipCache::addToCache(const ZLFile &file) {
	if (contains(file.path())) {
		return;
	}

	std::map<std::string,int> &offsetMap = myMap[file.path()];

	shared_ptr<ZLInputStream> stream = file.inputStream();

	if (stream.isNull() || !stream->open()) {
		return;
	}

	ZLZipHeader header;
	int offset = stream->offset();
	while (header.readFrom(*stream)) {
		std::string entryName(header.NameLength, '\0');
		if ((unsigned int)stream->read((char*)entryName.data(), header.NameLength) == header.NameLength) {
			offsetMap[entryName] = offset;
		}
		ZLZipHeader::skipEntry(*stream, header);
		offset = stream->offset();
	}
	stream->close();
}

bool ZLZipCache::contains(const std::string &fileName) {
	return myMap.find(fileName) != myMap.end();
}

int ZLZipCache::offset(const std::string &fileName, const std::string &entryName) {
	std::map<std::string,std::map<std::string,int> >::const_iterator it = myMap.find(fileName);
	if (it == myMap.end()) {
		return -1;
	}

	std::map<std::string,int>::const_iterator jt = it->second.find(entryName);
	return (jt != it->second.end()) ? jt->second : -1;
}
