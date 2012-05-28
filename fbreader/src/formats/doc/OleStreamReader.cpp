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

#include <ZLUnicodeUtil.h>
#include <ZLLogger.h>

#include "OleStream.h"

#include "OleStreamReader.h"
#include "DocBookReader.h"

//word's control chars:
static const unsigned short WORD_FOOTNOTE_MARK = 0x0002;
static const unsigned short WORD_TABLE_SEPARATOR = 0x0007;
static const unsigned short WORD_HORIZONTAL_TAB = 0x0009;
static const unsigned short WORD_HARD_RETURN = 0x000b;
static const unsigned short WORD_PAGE_BREAK = 0x000c;
static const unsigned short WORD_END_OF_PARAGRAPH =  0x000d;
static const unsigned short WORD_SHORT_DEFIS = 0x001e;
static const unsigned short WORD_SOFT_HYPHEN = 0x001f;
static const unsigned short WORD_START_EMB_HYPERLINK = 0x0013;
static const unsigned short WORD_SEPARATE_HYPERLINK_FROM_TEXT = 0x0014;
static const unsigned short WORD_END_EMB_HYPERLINK = 0x0015;
static const unsigned short WORD_ZERO_WIDTH_UNBREAKABLE_SPACE = 0xfeff;

//unicode values:
static const unsigned short FILE_SEPARATOR = 0x1c;
static const unsigned short LINE_FEED = 0x000a;
static const unsigned short SOFT_HYPHEN = 0xad;
static const unsigned short START_OF_HEADING = 0x0001;
static const unsigned short SPACE = 0x20;


OleStreamReader::OleStreamReader() {
	myBufIsUnicode = false;
}

bool OleStreamReader::readStream(OleStream &oleStream) {
	bool res = oleStream.open();
	if (!res) {
		ZLLogger::Instance().println("DocReader", "and doesn't open correct");
		return false;
	}
	ZLLogger::Instance().println("DocReader", "is word document!");

	//TODO maybe split this method on simplier parts

	int tabmode = 0;
	long offset = 0;
	int hyperlink_mode = 0;
	unsigned short c;
	bool paragraphEndSymbol = false;
	while (!oleStream.eof() && offset < oleStream.getLength()) {
		myBuffer.clear();
		paragraphEndSymbol = false;
		do {
			c=getUnicodeChar(oleStream, &offset, oleStream.getLength());
			if (tabmode) {
				tabmode=0;
				if (c==WORD_TABLE_SEPARATOR) {
					myBuffer.push_back(WORD_SHORT_DEFIS);
					continue;
				} else {
					myBuffer.push_back(FILE_SEPARATOR);
				}
			}
			if (c < 32) {
				switch (c) {
					case WORD_TABLE_SEPARATOR:
						tabmode = 1;
						break;
					case WORD_HARD_RETURN:
					case WORD_END_OF_PARAGRAPH:
						myBuffer.push_back(LINE_FEED);
						paragraphEndSymbol = true;
						break;
					case WORD_PAGE_BREAK:
						myBuffer.push_back(c);
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
						myBuffer.push_back(c);
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
						 // If any other control char, then current aragraph is discarded
						 myBuffer.clear();
				}
			} else if (c == WORD_ZERO_WIDTH_UNBREAKABLE_SPACE) {
				//skip
			} else {
				myBuffer.push_back(c);
			}
		} while (!oleStream.eof() && !paragraphEndSymbol);
		if (!myBuffer.empty()) {
			parapgraphHandler(convertToUtf8String(myBuffer));
		}
	}
	return 0;
}

int OleStreamReader::getUnicodeChar(OleStream& stream,long *offset,long fileend) {
	//TODO remove sending offset by pointer (bad style)
	static const long BLOCK_SIZE = 256;
	int count,i,u;
	char c;
	if ((i=(*offset)%BLOCK_SIZE) == 0) {
		count=stream.read(myTmpBuffer,1,BLOCK_SIZE);
		memset(myTmpBuffer+count,0,BLOCK_SIZE - count);
		myBufIsUnicode = false;
		if (*offset+(long)count > fileend) {
			count = fileend - (*offset);
		}
		while (i < count) {
			c=myTmpBuffer[i++];
			//TODO does it a reliable way to check on unicode?
			if ( (c == SPACE || c == WORD_END_OF_PARAGRAPH || ispunct(c)) && i<count && myTmpBuffer[i] == 0) {
				myBufIsUnicode = true;
				break;
			}
			i++;
		}
		i=0;
	}

	if (myBufIsUnicode) {
		u = myTmpBuffer[i] | myTmpBuffer[i+1]<<8;
		(*offset)+=2;
	} else {
		//TODO implement support for not unicodes 256 bytes blocks
		u=0; //to_unicode(source_charset,read_buf[i]);
		//(*offset)++;
		(*offset) += BLOCK_SIZE;
	}
	return u;
}

std::string OleStreamReader::convertToUtf8String(std::vector<unsigned short int>& buffer) {
	ZLUnicodeUtil::Ucs2String ucs2string;
	for (size_t i = 0; i < buffer.size(); ++i) {
		ZLUnicodeUtil::Ucs2Char ch = (ZLUnicodeUtil::Ucs2Char)(buffer.at(i));
		ucs2string.push_back(ch);
	}
	std::string to;
	ZLUnicodeUtil::ucs2ToUtf8(to, ucs2string);
	return to;
}
