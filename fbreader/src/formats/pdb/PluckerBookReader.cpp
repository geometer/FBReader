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

#include <algorithm>
#include <vector>
#include <cctype>

#include <ZLZDecompressor.h>
#include <ZLStringUtil.h>
#include <ZLUnicodeUtil.h>
#include <ZLImage.h>
#include <ZLFileImage.h>
#include <ZLFile.h>

#include "PdbReader.h"
#include "PluckerBookReader.h"
#include "DocDecompressor.h"
#include "PluckerImages.h"
#include "../../bookmodel/BookModel.h"

PluckerBookReader::PluckerBookReader(const std::string &filePath, BookModel &model, const std::string &encoding) : BookReader(model), EncodedTextReader(encoding), myFilePath(filePath), myFont(FT_REGULAR) {
	myCharBuffer = new char[65535];
	myForcedEntry = 0;
}

PluckerBookReader::~PluckerBookReader() {
	delete[] myCharBuffer;
}

void PluckerBookReader::safeAddControl(FBTextKind kind, bool start) {
	if (myParagraphStarted) {
		addControl(kind, start);
	} else {
		myDelayedControls.push_back(std::pair<FBTextKind,bool>(kind, start));
	}
}

void PluckerBookReader::safeAddHyperlinkControl(const std::string &id) {
	if (myParagraphStarted) {
		addHyperlinkControl(INTERNAL_HYPERLINK, id);
	} else {
		myDelayedHyperlinks.push_back(id);
	}
}

void PluckerBookReader::safeBeginParagraph() {
	if (!myParagraphStarted) {
		myParagraphStarted = true;
		myBufferIsEmpty = true;
		beginParagraph();
		if (!myParagraphStored) {
			myParagraphVector->push_back(model().bookTextModel()->paragraphsNumber() - 1);
			myParagraphStored = true;
		}
		for (std::vector<std::pair<FBTextKind,bool> >::const_iterator it = myDelayedControls.begin(); it != myDelayedControls.end(); ++it) {
			addControl(it->first, it->second);
		}
		if (myForcedEntry != 0) {
			addControl(*myForcedEntry);
		} else {
			addControl(REGULAR, true);
		}
		for (std::vector<std::string>::const_iterator it = myDelayedHyperlinks.begin(); it != myDelayedHyperlinks.end(); ++it) {
			addHyperlinkControl(INTERNAL_HYPERLINK, *it);
		}
		myDelayedHyperlinks.clear();
	}
}


void PluckerBookReader::safeEndParagraph() {
	if (myParagraphStarted) {
		if (myBufferIsEmpty) {
			static const std::string SPACE = " ";
			addData(SPACE);
		}
		endParagraph();
		myParagraphStarted = false;
	}
}

void PluckerBookReader::processHeader(FontType font, bool start) {
	if (start) {
		enterTitle();
		FBTextKind kind;
		switch (font) {
			case FT_H1:
				kind = H1;
				break;
			case FT_H2:
				kind = H2;
				break;
			case FT_H3:
				kind = H3;
				break;
			case FT_H4:
				kind = H4;
				break;
			case FT_H5:
				kind = H5;
				break;
			case FT_H6:
			default:
				kind = H6;
				break;
		}
		pushKind(kind);
	} else {
		popKind();
		exitTitle();
	}
};

void PluckerBookReader::setFont(FontType font, bool start) {
	switch (font) {
		case FT_REGULAR:
			break;
		case FT_H1:
		case FT_H2:
		case FT_H3:
		case FT_H4:
		case FT_H5:
		case FT_H6:
			processHeader(font, start);
			break;
		case FT_BOLD:
			safeAddControl(BOLD, start);
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

void PluckerBookReader::changeFont(FontType font) {
	if (myFont == font) {
		return;
	}
	setFont(myFont, false);
	myFont = font;
	setFont(myFont, true);
}

/*
static void listParameters(char *ptr) {
	int argc = ((unsigned char)*ptr) % 8;
	std::cerr << (int)(unsigned char)*ptr << "(";	
	for (int i = 0; i < argc - 1; ++i) {
		++ptr;
		std::cerr << (int)*ptr << ", ";	
	}
	if (argc > 0) {
		++ptr;
		std::cerr << (int)*ptr;	
	}
	std::cerr << ")\n";	
}
*/

static unsigned int twoBytes(char *ptr) {
	return 256 * (unsigned char)*ptr + (unsigned char)*(ptr + 1);
}

static std::string fromNumber(unsigned int num) {
	std::string str;
	ZLStringUtil::appendNumber(str, num);
	return str;
}

void PluckerBookReader::processTextFunction(char *ptr) {
	switch ((unsigned char)*ptr) {
		case 0x08:
			safeAddControl(INTERNAL_HYPERLINK, false);
			break;
		case 0x0A:
			safeAddHyperlinkControl(fromNumber(twoBytes(ptr + 1)));
			break;
		case 0x0C:
		{
			int sectionNum = twoBytes(ptr + 1);
			int paragraphNum = twoBytes(ptr + 3);
			safeAddHyperlinkControl(fromNumber(sectionNum) + '#' + fromNumber(paragraphNum));
			myReferencedParagraphs.insert(std::pair<int,int>(sectionNum, paragraphNum));
			break;
		}
		case 0x11:
			changeFont((FontType)*(ptr + 1));
			break;
		case 0x1A:
			safeBeginParagraph();
			addImageReference(fromNumber(twoBytes(ptr + 1)));
			break;
		case 0x22:
			if (!myParagraphStarted) {
				if (myForcedEntry == 0) {
					myForcedEntry = new ZLTextForcedControlEntry();
				}
				myForcedEntry->setLeftIndent(*(ptr + 1));
				myForcedEntry->setRightIndent(*(ptr + 2));
			}
			break;
		case 0x29:
			if (!myParagraphStarted) {
				if (myForcedEntry == 0) {
					myForcedEntry = new ZLTextForcedControlEntry();
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
			char utf8[4];
			int len = ZLUnicodeUtil::ucs2ToUtf8(utf8, twoBytes(ptr + 2));
			safeBeginParagraph();
			addData(std::string(utf8, len));
			myBufferIsEmpty = false;
			myBytesToSkip = *(ptr + 1);
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
			//std::cerr << "Oops... function #" << (int)(unsigned char)*ptr << "\n";
			break;
	}
}

void PluckerBookReader::processTextParagraph(char *start, char *end) {
	changeFont(FT_REGULAR);
	while (popKind()) {}

	myParagraphStarted = false;
	myBytesToSkip = 0;

	char *textStart = start;
	bool functionFlag = false;
	for (char *ptr = start; ptr < end; ++ptr) {
		if (*ptr == 0) {
			functionFlag = true;
			if (ptr > textStart) {
				safeBeginParagraph();
				myConvertedTextBuffer.erase();
				myConverter->convert(myConvertedTextBuffer, textStart, ptr);
				addData(myConvertedTextBuffer);
				myBufferIsEmpty = false;
			}
		} else if (functionFlag) {
			int paramCounter = ((unsigned char)*ptr) % 8;
			if (end - ptr > paramCounter) {
				processTextFunction(ptr);
				ptr += paramCounter;
			} else {
				ptr = end - 1;
			}
			functionFlag = false;
			if (myBytesToSkip > 0) {
				ptr += myBytesToSkip;
				myBytesToSkip = 0;
			}
			textStart = ptr + 1;
		} else {
			if ((unsigned char)*ptr == 0xA0) {
				*ptr = 0x20;
			}
			if (!myParagraphStarted && (textStart == ptr) && isspace((unsigned char)*ptr)) {
				++textStart;
			}
		}
	}
	if (end > textStart) {
		safeBeginParagraph();
		myConvertedTextBuffer.erase();
		myConverter->convert(myConvertedTextBuffer, textStart, end);
		addData(myConvertedTextBuffer);
		myBufferIsEmpty = false;
	}
	safeEndParagraph();
	if (myForcedEntry != 0) {
		delete myForcedEntry;
		myForcedEntry = 0;
	}
	myDelayedControls.clear();
}

void PluckerBookReader::processTextRecord(size_t size, const std::vector<int> &pars) {
	char *start = myCharBuffer;
	char *end = myCharBuffer;

	for (std::vector<int>::const_iterator it = pars.begin(); it != pars.end(); ++it) {
		start = end;
		end = start + *it;
		if (end > myCharBuffer + size) {
			return;
		}
		myParagraphStored = false;
		processTextParagraph(start, end);
		if (!myParagraphStored) {
			myParagraphVector->push_back(-1);
		}
	}
}

void PluckerBookReader::readRecord(size_t recordSize) {
	unsigned short uid;
	PdbUtil::readUnsignedShort(*myStream, uid);
	if (uid == 1) {
		PdbUtil::readUnsignedShort(*myStream, myCompressionVersion);
	} else {
		unsigned short paragraphs;
		PdbUtil::readUnsignedShort(*myStream, paragraphs);

		unsigned short size;
		PdbUtil::readUnsignedShort(*myStream, size);

		unsigned char type;
		myStream->read((char*)&type, 1);

		unsigned char flags;
		myStream->read((char*)&flags, 1);

		switch (type) {
			case 0: // text (TODO: found sample file and test this code)
			case 1: // compressed text
			{
				std::vector<int> pars;
				for (int i = 0; i < paragraphs; ++i) {
					unsigned short pSize;
					PdbUtil::readUnsignedShort(*myStream, pSize);
					pars.push_back(pSize);
					myStream->seek(2, false);
				}

				bool doProcess = false;
				if (type == 0) {
					doProcess = myStream->read(myCharBuffer, size) == size;
				} else if (myCompressionVersion == 1) {
					doProcess =
						DocDecompressor().decompress(*myStream, myCharBuffer, recordSize - 8 - 4 * paragraphs, size) == size;
				} else if (myCompressionVersion == 2) {
					myStream->seek(2, false);
					doProcess =
						ZLZDecompressor(recordSize - 10 - 4 * paragraphs).
							decompress(*myStream, myCharBuffer, size) == size;
				}
				if (doProcess) {
					addHyperlinkLabel(fromNumber(uid));
					myParagraphVector = &myParagraphMap[uid];
					processTextRecord(size, pars);
					if ((flags & 0x1) == 0) {
						insertEndOfTextParagraph();
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
					image = new ZLFileImage(mime, myFilePath, myStream->offset(), recordSize - 8);
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
				PdbUtil::readUnsignedShort(*myStream, typeCode);
				//std::cerr << "type = " << (int)type << "; ";
				//std::cerr << "typeCode = " << typeCode << "\n";
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
				PdbUtil::readUnsignedShort(*myStream, columns);
				PdbUtil::readUnsignedShort(*myStream, rows);
				PluckerMultiImage *image = new PluckerMultiImage(rows, columns, model().imageMap());
				for (int i = 0; i < size / 2 - 2; ++i) {
					unsigned short us;
					PdbUtil::readUnsignedShort(*myStream, us);
					image->addId(fromNumber(us));
				}
				addImage(fromNumber(uid), image);
				break;
			}
			default:
				//std::cerr << "type = " << (int)type << "\n";
				break;
		}
	}
}

bool PluckerBookReader::readDocument() {
	myStream = ZLFile(myFilePath).inputStream();
	if (myStream.isNull() || !myStream->open()) {
		return false;
	}

	PdbHeader header;
	if (!header.read(myStream)) {
		myStream->close();
		return false;
	}

	setMainTextModel();
	myFont = FT_REGULAR;

	for (std::vector<unsigned long>::const_iterator it = header.Offsets.begin(); it != header.Offsets.end(); ++it) {
		size_t currentOffset = myStream->offset();
		if (currentOffset > *it) {
			break;
		}
		myStream->seek(*it - currentOffset, false);
		if (myStream->offset() != *it) {
			break;
		}
		size_t recordSize = ((it != header.Offsets.end() - 1) ? *(it + 1) : myStream->sizeOfOpened()) - *it;
		readRecord(recordSize);
	}
	myStream->close();

	for (std::set<std::pair<int,int> >::const_iterator it = myReferencedParagraphs.begin(); it != myReferencedParagraphs.end(); ++it) {
		std::map<int,std::vector<int> >::const_iterator jt = myParagraphMap.find(it->first);
		if (jt != myParagraphMap.end()) {
			for (unsigned int k = it->second; k < jt->second.size(); ++k) {
				if (jt->second[k] != -1) {
					addHyperlinkLabel(fromNumber(it->first) + '#' + fromNumber(it->second), jt->second[k]);
					break;
				}
			}
		}
	}
	myReferencedParagraphs.clear();
	myParagraphMap.clear();
	return true;
}
