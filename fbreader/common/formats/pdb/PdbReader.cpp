/*
 * FBReader -- electronic book reader
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

#include <iostream>
#include <vector>

#include "PdbReader.h"

static void readUnsignedShort(shared_ptr<ZLInputStream> stream, unsigned short &N) {
	stream->read((char*)&N + 1, 1);
	stream->read((char*)&N, 1);
}

static void readUnsignedLong(shared_ptr<ZLInputStream> stream, unsigned long &N) {
	stream->read((char*)&N + 3, 1);
	stream->read((char*)&N + 2, 1);
	stream->read((char*)&N + 1, 1);
	stream->read((char*)&N, 1);
}

struct PdbHeader {
	std::string DocName;
	unsigned short Flags;
	std::string Id;

	bool read(shared_ptr<ZLInputStream> stream) FORMATS_SECTION;
};

bool PdbHeader::read(shared_ptr<ZLInputStream> stream) {
	size_t startOffset = stream->offset();

	char docName[33];
	stream->read(docName, 32);
	docName[32] = '\0';
	DocName = docName;

	readUnsignedShort(stream, Flags);

	stream->seek(26);
	
	char id[9];
	stream->read(id, 8);
	id[8] = '\0';
	Id = id;
	stream->seek(4);
	return stream->offset() == startOffset + 72;
}

bool PdbReader::readDocument(shared_ptr<ZLInputStream> stream) {
	if (stream.isNull() || !stream->open()) {
		return false;
	}

	PdbHeader header;
	if (!header.read(stream)) {
		stream->close();
		return false;
	}

	std::cerr << "name = " << header.DocName << "\n";
	std::cerr << "id = " << header.Id << "\n";

	bool code = false;
	if (header.Id == "DataPlkr") {
		std::vector<unsigned long> offsets;
		// record-id list
		stream->seek(4);
		unsigned short numRecords;
		::readUnsignedShort(stream, numRecords);
		offsets.reserve(numRecords);

		for (int i = 0; i < numRecords; i++) {
			unsigned long recordOffset;
			::readUnsignedLong(stream, recordOffset);
			offsets.push_back(recordOffset);
			stream->seek(4);
		}
		stream->seek(2);
		for (std::vector<unsigned long>::const_iterator it = offsets.begin(); it != offsets.end(); it++) {
			size_t currentOffset = stream->offset();
			if (currentOffset <= *it) {
				stream->seek(*it - currentOffset);
				if (stream->offset() != *it) {
					break;
				}
				std::cerr << "currentOffset = " << stream->offset() << "\n";
				unsigned short uid;
				readUnsignedShort(stream, uid);
				std::cerr << "uid = " << uid << "\n";
				if (uid > 1) {
					stream->seek(4);
					unsigned char type;
					stream->read((char*)&type, 1);
					std::cerr << "type = " << (int)type << "\n";
					stream->seek(1);
					if (type == 10) {
						unsigned short typeCode;
						readUnsignedShort(stream, typeCode);
						std::cerr << "typeCode = " << typeCode << "\n";
					}
				}
			}
		}
	} else if (header.Id == "TEXtREAd") {
	}

	stream->close();
	return code;
}
