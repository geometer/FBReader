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

#include "ZLZip.h"
#include "ZLZipHeader.h"
#include "../ZLFile.h"

void ZLZipDir::collectFiles(std::vector<std::string> &names, bool) {
	shared_ptr<ZLInputStream> stream = ZLFile(path()).inputStream();

	if (!stream.isNull() && stream->open()) {
		ZLZipHeader header;
		while (header.readFrom(*stream)) {
			char *buffer = new char[header.NameLength];
			if ((unsigned int)stream->read(buffer, header.NameLength) == header.NameLength) {
				std::string entryName;
				entryName.append(buffer, header.NameLength);
				names.push_back(entryName);
			}
			delete[] buffer;
			ZLZipHeader::skipEntry(*stream, header);
		}
		stream->close();
	}
}

std::string ZLZipDir::delimiter() const {
	return ":";
}
