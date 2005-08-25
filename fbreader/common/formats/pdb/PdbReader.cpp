/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <iostream>

#include <algorithm>
#include <vector>

#include <abstract/ZLZDecompressor.h>
#include <abstract/ZLStringUtil.h>
#include <abstract/ZLUnicodeUtil.h>
#include <abstract/ZLImage.h>
#include <abstract/ZLFSManager.h>

#include "PdbReader.h"
#include "DocDecompressor.h"
#include "PluckerImages.h"
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
	void processTextRecord(size_t size, const std::vector<int> &pars);
	void processTextParagraph(char *start, char *end);
	void processTextFunction(char *ptr);
	void setFont(FontType font, bool start);
	void changeFont(FontType font);

	void safeAddControl(TextKind kind, bool start);
	void safeAddHyperlinkControl(const std::string &id);
	void safeBeginParagraph();
	void safeEndParagraph();

private:
	std::string myFilePath;
	shared_ptr<ZLInputStream> myStream;
	FontType myFont;
	char *myCharBuffer;
	bool myParagraphStarted;
	ForcedControlEntry *myForcedEntry;
	std::vector<std::pair<TextKind,bool> > myDelayedControls;
	std::vector<std::string> myDelayedHyperlinks;
	int myBytesToSkip;
	unsigned short myCompressionVersion;
};

PluckerReader::PluckerReader(const std::string &filePath, shared_ptr<ZLInputStream> stream, BookModel &model) : BookReader(model), myFilePath(filePath), myStream(stream), myFont(FT_REGULAR) {
	myCharBuffer = new char[65535];
	myBytesToSkip = 0;
	myForcedEntry = 0;
}

PluckerReader::~PluckerReader() {
	delete[] myCharBuffer;
}

void PluckerReader::safeAddControl(TextKind kind, bool start) {
	if (myParagraphStarted) {
		addControl(kind, start);
	} else {
		myDelayedControls.push_back(std::pair<TextKind,bool>(kind, start));
	}
}

void PluckerReader::safeAddHyperlinkControl(const std::string &id) {
	if (myParagraphStarted) {
		addHyperlinkControl(HYPERLINK, id);
	} else {
		myDelayedControls.push_back(std::pair<TextKind,bool>(HYPERLINK, true));
		myDelayedHyperlinks.push_back(id);
	}
}

void PluckerReader::safeBeginParagraph() {
	if (!myParagraphStarted) {
		myParagraphStarted = true;
		beginParagraph();
		if (myForcedEntry != 0) {
			ForcedControlEntry *copy = new ForcedControlEntry();
			if (myForcedEntry->leftIndentSupported()) {
				copy->setLeftIndent(myForcedEntry->leftIndent());
			}
			if (myForcedEntry->rightIndentSupported()) {
				copy->setRightIndent(myForcedEntry->rightIndent());
			}
			if (myForcedEntry->alignmentTypeSupported()) {
				copy->setAlignmentType(myForcedEntry->alignmentType());
			}
			addControl(copy);
		} else {
			addControl(REGULAR, true);
		}
		unsigned int idIndex = 0;
		for (std::vector<std::pair<TextKind,bool> >::const_iterator it = myDelayedControls.begin(); it != myDelayedControls.end(); it++) {
			if ((it->first == HYPERLINK) && it->second && (idIndex < myDelayedHyperlinks.size())) {
				addHyperlinkControl(HYPERLINK, myDelayedHyperlinks[idIndex]);
				idIndex++;
			} else {
				addControl(it->first, it->second);
			}
		}
		myDelayedHyperlinks.clear();
	}
}

void PluckerReader::safeEndParagraph() {
	if (myParagraphStarted) {
		if (myBuffer.empty()) {
			addDataToBuffer(" ", 1);
		}
		endParagraph();
		myParagraphStarted = false;
	}
}

void PluckerReader::setFont(FontType font, bool start) {
	switch (font) {
		case FT_REGULAR:
			break;
		case FT_H1:
		case FT_H2:
		case FT_H3:
		case FT_H4:
		case FT_H5:
		case FT_H6:
			if (start) {
				enterTitle();
				pushKind(SECTION_TITLE);
			} else {
				popKind();
				exitTitle();
			}
			break;
		case FT_BOLD:
			safeAddControl(STRONG, start);
			break;
		case FT_TT:
			safeAddControl(CODE, start);
			break;
		case FT_SMALL:
			break;
		case FT_SUB:
			safeAddControl(SUB, start);
			break;
		case FT_SUP:
			safeAddControl(SUP, start);
			break;
	}
}

void PluckerReader::changeFont(FontType font) {
	if (myFont == font) {
		return;
	}
	setFont(myFont, false);
	myFont = font;
	setFont(myFont, true);
}

static void listParameters(char *ptr) {
	int argc = ((unsigned char)*ptr) % 8;
	std::cerr << (int)(unsigned char)*ptr << "(";	
	for (int i = 0; i < argc - 1; i++) {
		ptr++;
		std::cerr << (int)*ptr << ", ";	
	}
	if (argc > 0) {
		ptr++;
		std::cerr << (int)*ptr;	
	}
	std::cerr << ")\n";	
}

static unsigned int twoBytes(char *ptr) {
	return 256 * (unsigned char)*ptr + (unsigned char)*(ptr + 1);
}

static std::string fromNumber(unsigned int num) {
	std::string str;
	ZLStringUtil::appendNumber(str, num);
	return str;
}

void PluckerReader::processTextFunction(char *ptr) {
	switch ((unsigned char)*ptr) {
		case 0x08:
			safeAddControl(HYPERLINK, false);
			break;
		case 0x0A:
			safeAddHyperlinkControl(fromNumber(twoBytes(ptr + 1)));
			break;
		case 0x0C:
			safeAddHyperlinkControl(fromNumber(twoBytes(ptr + 1)) + "#" + fromNumber(twoBytes(ptr + 3)));
			break;
		case 0x11:
			changeFont((FontType)*(ptr + 1));
			break;
		case 0x1A:
			addImageReference(fromNumber(twoBytes(ptr + 1)));
			break;
		case 0x22:
			if (!myParagraphStarted) {
				if (myForcedEntry == 0) {
					myForcedEntry = new ForcedControlEntry();
				}
				myForcedEntry->setLeftIndent(*(ptr + 1));
				myForcedEntry->setRightIndent(*(ptr + 2));
			}
			break;
		case 0x29:
			if (!myParagraphStarted) {
				if (myForcedEntry == 0) {
					myForcedEntry = new ForcedControlEntry();
				}
				switch (*(ptr + 1)) {
					case 0: myForcedEntry->setAlignmentType(ALIGN_LEFT); break;
					case 1: myForcedEntry->setAlignmentType(ALIGN_RIGHT); break;
					case 2: myForcedEntry->setAlignmentType(ALIGN_CENTER); break;
					case 3: myForcedEntry->setAlignmentType(ALIGN_JUSTIFY); break;
				}
			}
			break;
		case 0x33: // just break line instead of horizontal rule (TODO: draw horizontal rule?)
			safeEndParagraph();
			break;
		case 0x38:
			safeEndParagraph();
			break;
		case 0x40: 
			safeAddControl(EMPHASIS, true);
			break;
		case 0x48:
			safeAddControl(EMPHASIS, false);
			break;
		case 0x53: // color setting is ignored
			break;
		case 0x5C:
			addImageReference(fromNumber(twoBytes(ptr + 3)));
			break;
		case 0x60: // underlined text is ignored
			break;
		case 0x68: // underlined text is ignored
			break;
		case 0x70: // strike-through text is ignored
			break;
		case 0x78: // strike-through text is ignored
			break;
		case 0x83: 
		{
			static char utf8[4];
			int len = ZLUnicodeUtil::ucs2ToUtf8(utf8, twoBytes(ptr + 2));
			safeBeginParagraph();
			addDataToBuffer(utf8, len);
			myBytesToSkip = (unsigned char)*(ptr + 1);
			break;
		}
		case 0x85: // TODO: process 4-byte unicode character
			break;
		case 0x8E: // custom font operations are ignored
		case 0x8C:
		case 0x8A:
		case 0x88:
			break;
		case 0x90: // TODO: add table processing
		case 0x92: // TODO: process table
		case 0x97: // TODO: process table
			break;
		default: // this should be impossible
			break;
	}
}

void PluckerReader::processTextParagraph(char *start, char *end) {
	changeFont(FT_REGULAR);
	while (popKind()) {}

	myParagraphStarted = false;

	char *textStart = start;
	bool functionFlag = false;
	char *ptr = start;
	for (; ptr != end; ptr++) {
		if (*ptr == 0) {
			functionFlag = true;
			if (ptr != textStart) {
				safeBeginParagraph();
				addDataToBuffer(textStart, ptr - textStart);
			}
		} else if (functionFlag) {
			int paramCounter = ((unsigned char)*ptr) % 8;
			if (end - ptr > paramCounter + 1) {
				processTextFunction(ptr);
				ptr += paramCounter;
			} else {
				ptr = end - 1;
			}
			functionFlag = false;
			textStart = ptr + 1 + myBytesToSkip;
			myBytesToSkip = 0;
		} else {
			if ((unsigned char)*ptr == 0xA0) {
				*ptr = 0x20;
			}
			if (!myParagraphStarted && isspace(*ptr) && (textStart == ptr)) {
				textStart++;
			}
		}
	}
	if (ptr != textStart) {
		safeBeginParagraph();
		addDataToBuffer(textStart, ptr - textStart);
	}
	safeEndParagraph();
	if (myForcedEntry != 0) {
		delete myForcedEntry;
		myForcedEntry = 0;
	}
	myDelayedControls.clear();
}

void PluckerReader::processTextRecord(size_t size, const std::vector<int> &pars) {
	char *start = myCharBuffer;
	char *end = myCharBuffer;

	for (std::vector<int>::const_iterator it = pars.begin(); it != pars.end(); it++) {
		start = end;
		end = start + *it;
		if (end > myCharBuffer + size) {
			return;
		}
		processTextParagraph(start, end);
	}
}

void PluckerReader::readRecord(size_t recordSize) {
	unsigned short uid;
	readUnsignedShort(myStream, uid);
	if (uid == 1) {
		readUnsignedShort(myStream, myCompressionVersion);
	} else {
		unsigned short paragraphs;
		readUnsignedShort(myStream, paragraphs);

		unsigned short size;
		readUnsignedShort(myStream, size);

		unsigned char type;
		myStream->read((char*)&type, 1);
		//std::cerr << "type = " << (int)type << "\n";

		unsigned char flags;
		myStream->read((char*)&flags, 1);

		switch (type) {
			case 0: // text (TODO: found sample file and test this code)
				std::cerr << "type = " << (int)type << "\n";
			case 1: // compressed text
			{
				std::vector<int> pars;
				for (int i = 0; i < paragraphs; i++) {
					unsigned short pSize;
					readUnsignedShort(myStream, pSize);
					pars.push_back(pSize);
					myStream->seek(2);
				}

				bool doProcess = false;
				if (type == 0) {
					doProcess = myStream->read(myCharBuffer, size) == size;
				} else if (myCompressionVersion == 1) {
					doProcess =
						DocDecompressor().decompress(*myStream, myCharBuffer, recordSize - 8 - 4 * paragraphs) == size;
				} else if (myCompressionVersion == 2) {
					myStream->seek(2);
					doProcess =
						ZLZDecompressor(recordSize - 10 - 4 * paragraphs).
							decompress(*myStream, myCharBuffer, size) == size;
				}
				if (doProcess) {
					addHyperlinkLabel(fromNumber(uid));
					processTextRecord(size, pars);
					if ((flags & 0x1) == 0) {
						insertEndOfSectionParagraph();
					}
				}
				break;
			}
			case 2: // image
			case 3: // compressed image
			{
				static const std::string mime = "image/palm";
				ZLImage *image = 0;
				if (type == 2) {
					image = new FileImage(mime, myFilePath, myStream->offset(), recordSize - 8);
				} else if (myCompressionVersion == 1) {
					image = new DocCompressedFileImage(mime, myFilePath, myStream->offset(), recordSize - 8);
				} else if (myCompressionVersion == 2) {
					image = new ZCompressedFileImage(mime, myFilePath, myStream->offset() + 2, recordSize - 10);
				}
				if (image != 0) {
					addImage(fromNumber(uid), image);
				}
				break;
			}
			case 9: // category record is ignored
				break;
			case 10:
				unsigned short typeCode;
				readUnsignedShort(myStream, typeCode);
				std::cerr << "type = " << (int)type << "; ";
				std::cerr << "typeCode = " << typeCode << "\n";
				break;
			case 11: // style sheet record is ignored
				break;
			case 12: // font page record is ignored
				break;
			case 13: // TODO: process tables
			case 14: // TODO: process tables
				break;
			case 15: // multiimage
			{
				unsigned short columns;
				unsigned short rows;
				::readUnsignedShort(myStream, columns);
				::readUnsignedShort(myStream, rows);
				PluckerMultiImage *image = new PluckerMultiImage(rows, columns, model().imageMap());
				for (int i = 0; i < size / 2 - 2; i++) {
					unsigned short us;
					::readUnsignedShort(myStream, us);
					image->addId(fromNumber(us));
				}
				addImage(fromNumber(uid), image);
				break;
			}
			default:
				std::cerr << "type = " << (int)type << "\n";
				break;
		}
	}
}

bool PluckerReader::readDocument() {
	setMainTextModel();
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
