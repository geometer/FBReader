/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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


#include <stdio.h>
#include <ctype.h>

#include <string.h>
#include <stdio.h>

#include <ZLLogger.h>

#include "OleMainStream.h"

#include "OleStreamReader.h"
#include "DocBookReader.h"
#include "OleUtil.h"

//word's control chars:
const ZLUnicodeUtil::Ucs2Char OleStreamReader::WORD_FOOTNOTE_MARK = 0x0002;
const ZLUnicodeUtil::Ucs2Char OleStreamReader::WORD_TABLE_SEPARATOR = 0x0007;
const ZLUnicodeUtil::Ucs2Char OleStreamReader::WORD_HORIZONTAL_TAB = 0x0009;
const ZLUnicodeUtil::Ucs2Char OleStreamReader::WORD_HARD_LINEBREAK = 0x000b;
const ZLUnicodeUtil::Ucs2Char OleStreamReader::WORD_PAGE_BREAK = 0x000c;
const ZLUnicodeUtil::Ucs2Char OleStreamReader::WORD_END_OF_PARAGRAPH =  0x000d;
const ZLUnicodeUtil::Ucs2Char OleStreamReader::WORD_SHORT_DEFIS = 0x001e;
const ZLUnicodeUtil::Ucs2Char OleStreamReader::WORD_SOFT_HYPHEN = 0x001f;
const ZLUnicodeUtil::Ucs2Char OleStreamReader::WORD_START_FIELD = 0x0013;
const ZLUnicodeUtil::Ucs2Char OleStreamReader::WORD_SEPARATOR_FIELD = 0x0014;
const ZLUnicodeUtil::Ucs2Char OleStreamReader::WORD_END_FIELD = 0x0015;
const ZLUnicodeUtil::Ucs2Char OleStreamReader::WORD_ZERO_WIDTH_UNBREAKABLE_SPACE = 0xfeff;

//unicode values:
const ZLUnicodeUtil::Ucs2Char OleStreamReader::NULL_SYMBOL = 0x0;
const ZLUnicodeUtil::Ucs2Char OleStreamReader::FILE_SEPARATOR = 0x1c;
const ZLUnicodeUtil::Ucs2Char OleStreamReader::LINE_FEED = 0x000a;
const ZLUnicodeUtil::Ucs2Char OleStreamReader::SOFT_HYPHEN = 0xad;
const ZLUnicodeUtil::Ucs2Char OleStreamReader::START_OF_HEADING = 0x0001;
const ZLUnicodeUtil::Ucs2Char OleStreamReader::SPACE = 0x20;
const ZLUnicodeUtil::Ucs2Char OleStreamReader::SHORT_DEFIS = 0x2D;
const ZLUnicodeUtil::Ucs2Char OleStreamReader::VERTICAL_LINE = 0x7C;


OleStreamReader::OleStreamReader(const std::string &encoding) :
	myEncoding(encoding) {
	clear();
}

void OleStreamReader::clear() {
	myBuffer.clear();
	myCurBufferPosition = 0;
	myNextPieceNumber = 0;
}

bool OleStreamReader::readStream(OleMainStream &oleMainStream) {
	clear();
	bool res = oleMainStream.open();
	if (!res) {
		ZLLogger::Instance().println("OleStreamReader", "doesn't open correct");
		return false;
	}
	ZLUnicodeUtil::Ucs2Char ucs2char;
	bool tabMode = false;
	while (getUcs2Char(oleMainStream, ucs2char)) {
		if (ucs2char < 32) {
			//printf("[0x%x]", ucs2char); //debug output
		}

		if (tabMode) {
			tabMode = false;
			if (ucs2char == WORD_TABLE_SEPARATOR) {
				handleTableEndRow();
				continue;
			} else {
				handleTableSeparator();
			}
		}

		if (ucs2char < 32) {
			if (ucs2char == NULL_SYMBOL) {
				//ignore 0x0 symbols
				continue;
			} else	if (ucs2char == WORD_HARD_LINEBREAK) {
				//printf("\n"); //debug output
				handleHardLinebreak();
			} else if (ucs2char == 	WORD_END_OF_PARAGRAPH) {
				//printf("\n"); //debug output
				handleParagraphEnd();
			} else if (ucs2char == WORD_PAGE_BREAK) {
				handlePageBreak();
			} else if (ucs2char == WORD_TABLE_SEPARATOR) {
				tabMode = true;
				continue;
			} else if (ucs2char == WORD_FOOTNOTE_MARK) {
				handleFootNoteMark();
			} else if (ucs2char == WORD_START_FIELD) {
				handleStartField();
			} else if (ucs2char == WORD_SEPARATOR_FIELD) {
				handleSeparatorField();
			} else if (ucs2char == WORD_END_FIELD) {
				handleEndField();
			} else if (ucs2char == START_OF_HEADING) {
				handleStartOfHeading();
			} else {
				// If any other control char, then current paragraph is discarded
				handleOtherControlChar(ucs2char);
			}
		} else if (ucs2char == WORD_ZERO_WIDTH_UNBREAKABLE_SPACE) {
			continue; //skip
		} else {
			//debug output
			std::string utf8String;
			ZLUnicodeUtil::Ucs2String ucs2String;
			ucs2String.push_back(ucs2char);
			ZLUnicodeUtil::ucs2ToUtf8(utf8String, ucs2String);
			//printf("%s", utf8String.c_str());

			handleChar(ucs2char);
		}
	}
	return 0;
}

bool OleStreamReader::getUcs2Char(OleMainStream& stream, ZLUnicodeUtil::Ucs2Char& ucs2char) {
	if (myCurBufferPosition >= myBuffer.size()) {
		if (!fillBuffer(stream)) {
			return false;
		}
	}
	ucs2char = myBuffer.at(myCurBufferPosition++);
	return true;
}

bool OleStreamReader::fillBuffer(OleMainStream& stream) {
	const OleMainStream::Pieces& pieces = stream.getPieces();
	if (myNextPieceNumber >= pieces.size()) {
		return false; //end of reading
	}
	const OleMainStream::Piece& piece = pieces.at(myNextPieceNumber);
	char* textBuffer = new char[piece.length];
	stream.seek(piece.offset, true);
	stream.read(textBuffer, piece.length);

	myBuffer.clear();
	if (!piece.isANSI) {
		for (long i = 0; i < piece.length; i += 2) {
			ZLUnicodeUtil::Ucs2Char ch = OleUtil::getUShort(textBuffer, i);
			myBuffer.push_back(ch);
		}
	} else {
		if (myConverter.isNull()) {
			//lazy convertor loading, because documents can be in Unicode only and don't need to be converted
			ZLEncodingCollection &collection = ZLEncodingCollection::Instance();
			ZLEncodingConverterInfoPtr info = collection.info(myEncoding);
			myConverter = (!info.isNull()) ? info->createConverter() : collection.defaultConverter();
		}
		std::string utf8String;
		myConverter->convert(utf8String, std::string(textBuffer, piece.length));
		ZLUnicodeUtil::utf8ToUcs2(myBuffer, utf8String);
	}
	myCurBufferPosition = 0;
	++myNextPieceNumber;
	delete textBuffer;
	return true;
}
