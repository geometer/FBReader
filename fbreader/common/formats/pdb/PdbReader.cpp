/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include <abstract/ZLFSManager.h>

#include "PdbReader.h"
#include "../../bookmodel/BookModel.h"
#include "../../bookmodel/BookReader.h"

void PdbUtil::readUnsignedShort(shared_ptr<ZLInputStream> stream, unsigned short &N) {
	stream->read((char*)&N + 1, 1);
	stream->read((char*)&N, 1);
}

void PdbUtil::readUnsignedLong(shared_ptr<ZLInputStream> stream, unsigned long &N) {
	stream->read((char*)&N + 3, 1);
	stream->read((char*)&N + 2, 1);
	stream->read((char*)&N + 1, 1);
	stream->read((char*)&N, 1);
}

bool PdbHeader::read(shared_ptr<ZLInputStream> stream) {
	size_t startOffset = stream->offset();

	DocName.erase();
	DocName.append(32, '\0');
	stream->read((char*)DocName.data(), 32);

	PdbUtil::readUnsignedShort(stream, Flags);

	stream->seek(26);
	
	Id.erase();
	Id.append(8, '\0');
	stream->read((char*)Id.data(), 8);

	stream->seek(8);

	Offsets.clear();
	unsigned short numRecords;
	PdbUtil::readUnsignedShort(stream, numRecords);
	Offsets.reserve(numRecords);

	for (int i = 0; i < numRecords; ++i) {
		unsigned long recordOffset;
		PdbUtil::readUnsignedLong(stream, recordOffset);
		Offsets.push_back(recordOffset);
		stream->seek(4);
	}

	return stream->offset() == startOffset + 78 + 8 * numRecords;
}
