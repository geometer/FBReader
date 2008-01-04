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

#include <ZLFile.h>

#include "PdbReader.h"
#include "../../bookmodel/BookModel.h"
#include "../../bookmodel/BookReader.h"

void PdbUtil::readUnsignedShort(ZLInputStream &stream, unsigned short &N) {
	unsigned char data[2];
	stream.read((char*)data, 2);
	N = (((unsigned short)data[0]) << 8) + data[1];
	/*
	stream.read((char*)&N + 1, 1);
	stream.read((char*)&N, 1);
	*/
}

void PdbUtil::readUnsignedLong(ZLInputStream &stream, unsigned long &N) {
	unsigned char data[4];
	stream.read((char*)data, 4);
	N = (((unsigned long)data[0]) << 24) +
			(((unsigned long)data[1]) << 16) +
			(((unsigned long)data[2]) << 8) +
			(unsigned long)data[3];
	/*
	stream.read((char*)&N + 3, 1);
	stream.read((char*)&N + 2, 1);
	stream.read((char*)&N + 1, 1);
	stream.read((char*)&N, 1);
	*/
}

bool PdbHeader::read(shared_ptr<ZLInputStream> stream) {
	size_t startOffset = stream->offset();

	DocName.erase();
	DocName.append(32, '\0');
	stream->read((char*)DocName.data(), 32);

	PdbUtil::readUnsignedShort(*stream, Flags);

	stream->seek(26, false);
	
	Id.erase();
	Id.append(8, '\0');
	stream->read((char*)Id.data(), 8);

	stream->seek(8, false);

	Offsets.clear();
	unsigned short numRecords;
	PdbUtil::readUnsignedShort(*stream, numRecords);
	Offsets.reserve(numRecords);

	for (int i = 0; i < numRecords; ++i) {
		unsigned long recordOffset;
		PdbUtil::readUnsignedLong(*stream, recordOffset);
		Offsets.push_back(recordOffset);
		stream->seek(4, false);
	}

	return stream->offset() == startOffset + 78 + 8 * numRecords;
}
