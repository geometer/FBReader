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

#include "OleStream.h"

#include "OleStreamReader.h"
#include "DocBookReader.h"
#include "NumUtil.h"

//word's control chars:
static const ZLUnicodeUtil::Ucs2Char WORD_FOOTNOTE_MARK = 0x0002;
static const ZLUnicodeUtil::Ucs2Char  WORD_TABLE_SEPARATOR = 0x0007;
static const ZLUnicodeUtil::Ucs2Char WORD_HORIZONTAL_TAB = 0x0009;
static const ZLUnicodeUtil::Ucs2Char WORD_HARD_RETURN = 0x000b;
static const ZLUnicodeUtil::Ucs2Char WORD_PAGE_BREAK = 0x000c;
static const ZLUnicodeUtil::Ucs2Char WORD_END_OF_PARAGRAPH =  0x000d;
static const ZLUnicodeUtil::Ucs2Char WORD_SHORT_DEFIS = 0x001e;
static const ZLUnicodeUtil::Ucs2Char WORD_SOFT_HYPHEN = 0x001f;
static const ZLUnicodeUtil::Ucs2Char WORD_START_EMB_HYPERLINK = 0x0013;
static const ZLUnicodeUtil::Ucs2Char WORD_SEPARATE_HYPERLINK_FROM_TEXT = 0x0014;
static const ZLUnicodeUtil::Ucs2Char WORD_END_EMB_HYPERLINK = 0x0015;
static const ZLUnicodeUtil::Ucs2Char WORD_ZERO_WIDTH_UNBREAKABLE_SPACE = 0xfeff;

//unicode values:
static const ZLUnicodeUtil::Ucs2Char NULL_SYMBOL = 0x0;
static const ZLUnicodeUtil::Ucs2Char FILE_SEPARATOR = 0x1c;
static const ZLUnicodeUtil::Ucs2Char LINE_FEED = 0x000a;
static const ZLUnicodeUtil::Ucs2Char SOFT_HYPHEN = 0xad;
static const ZLUnicodeUtil::Ucs2Char START_OF_HEADING = 0x0001;
static const ZLUnicodeUtil::Ucs2Char SPACE = 0x20;


OleStreamReader::OleStreamReader(const std::string &encoding) : myEncoding(encoding) {
	myBufIsUnicode = false;
}

bool OleStreamReader::readStream(OleStream &oleStream) {
	bool res = oleStream.open();
	if (!res) {
		ZLLogger::Instance().println("DocReader", " doesn't open correct");
		return false;
	}
	//TODO maybe split this method on simplier parts

	int tabmode = 0;
	long offset = 0;
	int hyperlink_mode = 0;
	ZLUnicodeUtil::Ucs2Char ucs2char;
	bool paragraphEndSymbol = false;
	while (!oleStream.eof() && offset < oleStream.getLength()) {
		myBuffer.clear();
		paragraphEndSymbol = false;
		do {
			ucs2char = getUcs2Char(oleStream, &offset, oleStream.getLength());
			if (ucs2char == NULL_SYMBOL) {
				//ignore 0x0 symbols
				continue;
			}
			if (tabmode) {
				tabmode=0;
				if (ucs2char == WORD_TABLE_SEPARATOR) {
					myBuffer.push_back(WORD_SHORT_DEFIS);
					continue;
				} else {
					myBuffer.push_back(FILE_SEPARATOR);
				}
			}
			if (ucs2char < 32) {
				switch (ucs2char) {
					case WORD_TABLE_SEPARATOR:
						tabmode = 1;
						break;
					case WORD_HARD_RETURN:
					case WORD_END_OF_PARAGRAPH:
						myBuffer.push_back(LINE_FEED);
						paragraphEndSymbol = true;
						break;
					case WORD_PAGE_BREAK:
						myBuffer.push_back(ucs2char);
						break;
					case WORD_SHORT_DEFIS:
						myBuffer.push_back('-');
						break;
					case WORD_FOOTNOTE_MARK: 
						break;
					case WORD_SOFT_HYPHEN:
						myBuffer.push_back(SOFT_HYPHEN);
						break;
					case WORD_HORIZONTAL_TAB:
						myBuffer.push_back(ucs2char);
						 break;
					case WORD_START_EMB_HYPERLINK:
						 hyperlink_mode=1;
						 myBuffer.push_back(' ');
						 break;
					case WORD_SEPARATE_HYPERLINK_FROM_TEXT:
						 hyperlink_mode = 0;
						 /*fall through */
					case WORD_END_EMB_HYPERLINK:
						 /* just treat hyperlink separators as space */
						 myBuffer.push_back(' ');
						 break;
					case START_OF_HEADING:
						 if (hyperlink_mode) {
							break;
						 }
						 /* else fall through */
					default:
						 // If any other control char, then current paragraph is discarded
						 myBuffer.clear();
				}
			} else if (ucs2char == WORD_ZERO_WIDTH_UNBREAKABLE_SPACE) {
				//skip
			} else {
				myBuffer.push_back(ucs2char);
			}
		} while (!oleStream.eof() && !paragraphEndSymbol);
		if (!myBuffer.empty()) {
			std::string utf8String;
			ZLUnicodeUtil::ucs2ToUtf8(utf8String, myBuffer);
			parapgraphHandler(utf8String);
		}
	}
	return 0;
}

ZLUnicodeUtil::Ucs2Char OleStreamReader::getUcs2Char(OleStream& stream,long *offset,long fileend) {
	//TODO remove sending offset by pointer
	static const long BLOCK_SIZE = 256;
	int count,i;
	ZLUnicodeUtil::Ucs2Char u;
	char c;
	if ((i=(*offset)%BLOCK_SIZE) == 0) {
		count=stream.read(myTmpBuffer,1,BLOCK_SIZE);
		memset(myTmpBuffer+count,0,BLOCK_SIZE - count);
		myBufIsUnicode = false;
		if (*offset+(long)count > fileend) {
			count = fileend - (*offset);
		}
		while (i < count) {
			c = myTmpBuffer[i++];
			//does it a reliable way to check on unicode?
			if (myTmpBuffer[i] == 0 && (c == SPACE || c == WORD_END_OF_PARAGRAPH || ispunct(c)) && i<count) {
				myBufIsUnicode = true;
				break;
			}
			i++;
		}
		i=0;
	}

	if (myBufIsUnicode) {
		u = NumUtil::getUInt16(myTmpBuffer, i);
		(*offset)+=2;
	} else {
		if (myConverter.isNull()) {
			//lazy convertor loading, because documents can be in Unicode only and don't need to be converted
			ZLEncodingCollection &collection = ZLEncodingCollection::Instance();
			ZLEncodingConverterInfoPtr info = collection.info(myEncoding);
			myConverter = (!info.isNull()) ? info->createConverter() : collection.defaultConverter();
		}
		//TODO is there's a way to convert to ucs2string at once?
		std::string utf8String;
		myConverter->convert(utf8String, std::string(1,myTmpBuffer[i]));
		ZLUnicodeUtil::Ucs2String ucs2string;
		ZLUnicodeUtil::utf8ToUcs2(ucs2string, utf8String);
		if (ucs2string.empty()) {
			//in some word documents, there's 0x0 symbols in 256 not unicodes block exist -- ignore them
			u = NULL_SYMBOL;
		} else {
			u = ucs2string.at(0);
		}
		(*offset)+=1;
	}
	return u;
}
