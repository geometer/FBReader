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
#include <algorithm>
#include <vector>

#include <abstract/ZLZDecompressor.h>
#include <abstract/ZLStringUtil.h>
#include <abstract/ZLImage.h>
#include <abstract/ZLFSManager.h>

#include "PdbReader.h"
#include "../../bookmodel/BookModel.h"
#include "../../bookmodel/BookReader.h"

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

class PluckerReader : public BookReader {

public:
	PluckerReader(const std::string &filePath, shared_ptr<ZLInputStream> stream, BookModel &model);
	~PluckerReader();

	bool readDocument();

private:
	enum FontType {
		FT_REGULAR = 0,
		FT_H1 = 1,
		FT_H2 = 2,
		FT_H3 = 3,
		FT_H4 = 4,
		FT_H5 = 5,
		FT_H6 = 6,
		FT_BOLD = 7,
		FT_TT = 8,
		FT_SMALL = 9,
		FT_SUB = 10,
		FT_SUP = 11
	};

	void readRecord(size_t recordSize);
	void processCompressedTextRecord(size_t size);
	void changeFont(FontType font);

private:
	std::string myFilePath;
	shared_ptr<ZLInputStream> myStream;
	FontType myFont;
};

PluckerReader::PluckerReader(const std::string &filePath, shared_ptr<ZLInputStream> stream, BookModel &model) : BookReader(model), myFilePath(filePath), myStream(stream), myFont(FT_REGULAR) {
}

PluckerReader::~PluckerReader() {
}

void PluckerReader::changeFont(FontType font) {
	if (myFont == font) {
		return;
	}
	switch (myFont) {
		case FT_REGULAR:
			break;
		case FT_H1:
		case FT_H2:
		case FT_H3:
		case FT_H4:
		case FT_H5:
		case FT_H6:
			endParagraph();
			popKind();
			endContentsParagraph();
			exitTitle();
			beginParagraph();
			break;
		case FT_BOLD:
			addControl(STRONG, false);
			break;
		case FT_TT:
			addControl(CODE, false);
			break;
		case FT_SMALL:
			break;
		case FT_SUB:
			addControl(SUB, false);
			break;
		case FT_SUP:
			addControl(SUP, false);
			break;
	}
	myFont = font;
	switch (myFont) {
		case FT_REGULAR:
			break;
		case FT_H1:
		case FT_H2:
		case FT_H3:
		case FT_H4:
		case FT_H5:
		case FT_H6:
			endParagraph();
			insertEndOfSectionParagraph();
			enterTitle();
			beginContentsParagraph();
			pushKind(SECTION_TITLE);
			beginParagraph();
			break;
		case FT_BOLD:
			addControl(STRONG, true);
			break;
		case FT_TT:
			addControl(CODE, true);
			break;
		case FT_SMALL:
			break;
		case FT_SUB:
			addControl(SUB, true);
			break;
		case FT_SUP:
			addControl(SUP, true);
			break;
	}
}

void PluckerReader::processCompressedTextRecord(size_t size) {
	beginParagraph();

	ZLZDecompressor decompressor(size);
	char buffer[1024];
	size_t s;
	std::string record;
	do {
		s = decompressor.decompress(*myStream, buffer, 1024);
		if (s != 0) {
			record.append(buffer, s);
		}
	} while (s == 1024);

	const char *dataStart = record.data();
	const char *dataEnd = dataStart + record.length();
	const char *textStart = dataStart;
	bool functionFlag = false;
	for (const char *ptr = dataStart; ptr < dataEnd; ptr++) {
		if (functionFlag) {
			bool processed = false;
			int fn = (unsigned char)*ptr;
			switch (*ptr) {
				case 0x0A:
					{
						unsigned int recordId = 256 * (unsigned char)*(ptr + 1) + (unsigned char)*(ptr + 2);
						std::string id;
						ZLStringUtil::appendNumber(id, recordId);
						addHyperlinkControl(HYPERLINK, id);
					}
					ptr += 2;
					break;
				case 0x0C:
					{
						unsigned int recordId = 256 * (unsigned char)*(ptr + 1) + (unsigned char)*(ptr + 2);
						unsigned int paragraphId = 256 * (unsigned char)*(ptr + 3) + (unsigned char)*(ptr + 4);
						std::string id;
						ZLStringUtil::appendNumber(id, recordId);
						id += "#";
						ZLStringUtil::appendNumber(id, paragraphId);
						addHyperlinkControl(HYPERLINK, id);
						std::cerr << id << "\n";
					}
					ptr += 4;
					break;
				case 0x08:
					addControl(HYPERLINK, false);
					ptr += 0;
					break;
				case 0x11:
					ptr += 1;
					//std::cerr << "== " << (int)*ptr << "\n";
					changeFont((FontType)*ptr);
					processed = true;
					break;
				case 0x1A:
					{
						unsigned int id = 256 * (unsigned char)*(ptr + 1) + (unsigned char)*(ptr + 2);
						std::string strId;
						ZLStringUtil::appendNumber(strId, id);
						endParagraph();
						addImageReference(strId);
						beginParagraph();
					}
					ptr += 2;
					processed = true;
					break;
				case 0x22: ptr += 2; break;
				case 0x29:
					ptr += 1;
					processed = true;
					break;
				case 0x33:
					ptr += 3;
					endParagraph();
					beginParagraph(Paragraph::EMPTY_LINE_PARAGRAPH);
					endParagraph();
					beginParagraph();
					processed = true;
					break;
				case 0x38:
					endParagraph();
					beginParagraph();
					processed = true;
					break;
				case 0x40:
					addControl(EMPHASIS, true);
					processed = true;
					break;
				case 0x48:
					addControl(EMPHASIS, false);
					processed = true;
					break;
				case 0x53:
					ptr += 3;
					processed = true;
					break;
				case 0x5C: ptr += 4; break;
				case 0x60: ptr += 0; break;
				case 0x68: ptr += 0; break;
				case 0x70: ptr += 0; break;
				case 0x78: ptr += 0; break;
				case 0x83: ptr += 3; break;
				case 0x85: ptr += 5; break;
				case 0x8E: ptr += 6; break;
				case 0x8C: ptr += 4; break;
				case 0x8A: ptr += 2; break;
				case 0x88: ptr += 0; break;
				case 0x90: ptr += 0; break;
				case 0x92: ptr += 2; break;
				case 0x97: ptr += 7; break;
			}
			textStart = ptr + 1;
			if (!processed) {
				//std::cerr << "[" << fn << "]\n";
			}
			functionFlag = false;
		} else if (*ptr == 0) {
			functionFlag = true;
			if (textStart != ptr) {
				addDataToBuffer(textStart, ptr - textStart);
				//std::cerr << "{" << (int)(unsigned char)textStart[0] << "}\n";
				//std::cerr << "{" << (int)(unsigned char)textStart[1] << "}\n";
				//std::cerr << textStart;
			}
			textStart = ptr + 1;
		} else if (*ptr == (char)160) {
			if (textStart != ptr) {
				addDataToBuffer(textStart, ptr - textStart);
				//std::cerr << "{" << (int)(unsigned char)textStart[0] << "}\n";
				//std::cerr << "{" << (int)(unsigned char)textStart[1] << "}\n";
				//std::cerr << textStart;
			}
			textStart = ptr + 1;
			changeFont(FT_REGULAR);
			//endParagraph();
			//beginParagraph();
		}
	}
	endParagraph();
}

void PluckerReader::readRecord(size_t recordSize) {
	unsigned short uid;
	readUnsignedShort(myStream, uid);
	if (uid == 1) {
		unsigned short version;
		readUnsignedShort(myStream, version);
	} else {
		unsigned short paragraphs;
		readUnsignedShort(myStream, paragraphs);

		unsigned short size;
		readUnsignedShort(myStream, size);

		unsigned char type;
		myStream->read((char*)&type, 1);

		unsigned char flags;
		myStream->read((char*)&flags, 1);

		switch (type) {
			case 1: // compressed text
				myStream->seek(4 * paragraphs + 2);
				{
					std::string strId;
					ZLStringUtil::appendNumber(strId, uid);
					addHyperlinkLabel(strId);
					processCompressedTextRecord(recordSize - 10 - 4 * paragraphs);
				}
				break;
			case 3: // compressed image
				{
					myStream->seek(2);
					std::string strId;
					ZLStringUtil::appendNumber(strId, uid);
					addImage(strId, new ZLZCompressedFileImage("image/palm", myFilePath, myStream->offset(), recordSize - 10));
				}
				break;
			case 10:
				unsigned short typeCode;
				readUnsignedShort(myStream, typeCode);
				std::cerr << "type = " << (int)type << "; ";
				std::cerr << "typeCode = " << typeCode << "\n";
				break;
			case 15: // multiimage
				/*
				std::cerr << "uid = " << (int)uid << "; ";
				std::cerr << "type = " << (int)type << "; ";
				for (int i = 0; i < size / 2; i++) {
					unsigned short us;
					::readUnsignedShort(myStream, us);
					std::cerr << us << " ";
				}
				std::cerr << "\n";
				*/
				break;
			default:
				std::cerr << "type = " << (int)type << "\n";
				//std::cerr << "size = " << size << "\n";
				break;
		}
	}
}

bool PluckerReader::readDocument() {
	setMainTextModel();
	pushKind(REGULAR);
	myFont = FT_REGULAR;

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
		size_t recordSize = ((it != offsets.end() - 1) ? *(it + 1) : myStream->sizeOfOpened()) - *it;
		readRecord(recordSize);
	}
	return true;
}

bool PdbReader::readDocument(const std::string &myFilePath, BookModel &model) {
	shared_ptr<ZLInputStream> stream = ZLFile(myFilePath).inputStream();
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
		code = PluckerReader(myFilePath, stream, model).readDocument();
	} else if (header.Id == "TEXtREAd") {
	}

	stream->close();
	return code;
}
