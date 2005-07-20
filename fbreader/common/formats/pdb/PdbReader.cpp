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

class PluckerReader {

public:
	PluckerReader(shared_ptr<ZLInputStream> stream);
	~PluckerReader();

	bool readDocument();

private:
	void readRecord();

private:
	shared_ptr<ZLInputStream> myStream;
};

PluckerReader::PluckerReader(shared_ptr<ZLInputStream> stream) : myStream(stream) {
}

PluckerReader::~PluckerReader() {
}

void PluckerReader::readRecord() {
	unsigned short uid;
	readUnsignedShort(myStream, uid);
	std::cerr << "uid = " << uid << "; ";
	if (uid > 1) {
		unsigned short paragraphs;
		readUnsignedShort(myStream, paragraphs);
		std::cerr << "paragraphs = " << paragraphs << "; ";

		unsigned short size;
		readUnsignedShort(myStream, size);
		std::cerr << "size = " << size << "; ";

		unsigned char type;
		myStream->read((char*)&type, 1);
		std::cerr << "type = " << (int)type << "; ";

		unsigned char flags;
		myStream->read((char*)&flags, 1);
		std::cerr << "flags = " << (int)flags << "\n";

		/*
		if (type == 10) {
			unsigned short typeCode;
			readUnsignedShort(myStream, typeCode);
			std::cerr << "typeCode = " << typeCode << "\n";
		}
		*/
	}
}

bool PluckerReader::readDocument() {
	std::vector<unsigned long> offsets;
	// record-id list
	myStream->seek(4);
	unsigned short numRecords;
	::readUnsignedShort(myStream, numRecords);
	offsets.reserve(numRecords);

	for (int i = 0; i < numRecords; i++) {
		unsigned long recordOffset;
		::readUnsignedLong(myStream, recordOffset);
		offsets.push_back(recordOffset);
		myStream->seek(4);
	}
	myStream->seek(2);
	for (std::vector<unsigned long>::const_iterator it = offsets.begin(); it != offsets.end(); it++) {
		size_t currentOffset = myStream->offset();
		if (currentOffset > *it) {
			break;
		}
		myStream->seek(*it - currentOffset);
		if (myStream->offset() != *it) {
			break;
		}
		//std::cerr << "currentOffset = " << myStream->offset() << "\n";
		readRecord();
	}
	return false;
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

	//std::cerr << "name = " << header.DocName << "\n";
	//std::cerr << "id = " << header.Id << "\n";

	bool code = false;
	if (header.Id == "DataPlkr") {
		code = PluckerReader(stream).readDocument();
	} else if (header.Id == "TEXtREAd") {
	}

	stream->close();
	return code;
}
