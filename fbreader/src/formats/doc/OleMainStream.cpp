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

#include <cstdio>
#include <cstring> //for memset

#include <ZLLogger.h>
#include <ZLUnicodeUtil.h>

#include "OleUtil.h"
#include "OleStorage.h"

#include "OleMainStream.h"

OleMainStream::StyleInfo::StyleInfo() {
	(void)memset(this, 0, sizeof(*this));
	istd = ISTD_INVALID;
	istdNext = ISTD_INVALID;
	startAt = 1;
	listLevel = 9;
}

OleMainStream::CharInfo::CharInfo():
	offset(0),
	fontStyle(0),
	fontSize(20) {
}


OleMainStream::SectionInfo::SectionInfo() :
	charPos(0),
	newPage(true) {
}

OleMainStream::OleMainStream(shared_ptr<OleStorage> storage, OleEntry oleEntry, shared_ptr<ZLInputStream> stream) :
	OleStream(storage, oleEntry, stream) {
}

bool OleMainStream::open() {
	if (OleStream::open() == false) {
		return false;
	}

	static const size_t HEADER_SIZE = 768; //size of data in header of main stream
	char headerBuffer[HEADER_SIZE];
	seek(0, true);

	if (read(headerBuffer, HEADER_SIZE) != HEADER_SIZE) {
		return false;
	}

	bool result = readFIB(headerBuffer);
	if (!result) {
		return false;
	}

	//determining table stream number
	unsigned int tableNumber = (OleUtil::getU2Bytes(headerBuffer, 0xA) & 0x0200) ? 1 : 0;
	std::string tableName = tableNumber == 0 ? "0" : "1";
	tableName += "Table";
	OleEntry tableEntry;
	result = myStorage->getEntryByName(tableName, tableEntry);

	if (!result) {
		//cant't find table stream (that can be only in case if file format is below Word 7/8), so building simple table stream
		Piece piece = {myStartOfText, myEndOfText - myStartOfText, true}; //CHECK may be not all old documents have ANSI
		myPieces.push_back(piece);
		return true;
	}
	//printf("Table Name: %s\n", tableName.c_str());

	return readPieceTable(headerBuffer, tableEntry) &&
			readStylesheet(headerBuffer, tableEntry) &&
			readSectionsInfoTable(headerBuffer, tableEntry) &&
			readParagraphStyleTable(headerBuffer, tableEntry) &&
			readCharInfoTable(headerBuffer, tableEntry);

}

const OleMainStream::Pieces& OleMainStream::getPieces() const {
	return myPieces;
}

const OleMainStream::CharInfoList &OleMainStream::getCharInfoList() const {
	return myCharInfoList;
}

const OleMainStream::StyleInfoList &OleMainStream::getStyleInfoList() const {
	return myStyleInfoList;
}

const OleMainStream::SectionInfoList &OleMainStream::getSectionInfoList() const {
	return mySectionInfoList;
}

bool OleMainStream::readFIB(const char* headerBuffer) {
	int flags = OleUtil::getU2Bytes(headerBuffer, 0xA); //offset for flags

	if (flags & 0x0004) { //flag for complex format
		ZLLogger::Instance().println("OleMainStream", "This was fast-saved. Some information is lost");
		//lostInfo = (flags & 0xF0) >> 4);
	}

	if (flags & 0x1000) { //flag for using extending charset
		ZLLogger::Instance().println("OleMainStream", "File uses extended character set (get_word8_char)");
	} else {
		ZLLogger::Instance().println("OleMainStream", "File uses get_8bit_char character set");
	}

	if (flags & 0x100) { //flag for encrypted files
		ZLLogger::Instance().println("OleMainStream", "File is encrypted");
		// Encryption key = %08lx ; NumUtil::get4Bytes(header, 14)
		return false;
	}

	unsigned int charset = OleUtil::getU2Bytes(headerBuffer, 0x14); //offset for charset number
	if (charset && charset != 0x100) { //0x100 = default charset
		ZLLogger::Instance().println("OleMainStream", "Using not default character set %d");
	} else {
		ZLLogger::Instance().println("OleMainStream", "Using default character set");
	}

	myStartOfText = OleUtil::get4Bytes(headerBuffer, 0x18); //offset for start of text value
	myEndOfText = OleUtil::get4Bytes(headerBuffer, 0x1c); //offset for end of text value
	return true;
}

int OleMainStream::getLastCP(const char* buffer) {
	//getting count of Character Positions for different types of subdocuments in Main Stream
	int ccpText = OleUtil::get4Bytes(buffer, 0x004C); //text
	int ccpFtn = OleUtil::get4Bytes(buffer, 0x0050); //footnote subdocument
	int ccpHdd = OleUtil::get4Bytes(buffer, 0x0054); //header subdocument
	int ccpMcr = OleUtil::get4Bytes(buffer, 0x0058); //macro subdocument
	int ccpAtn = OleUtil::get4Bytes(buffer, 0x005C); //comment subdocument
	int ccpEdn = OleUtil::get4Bytes(buffer, 0x0060); //endnote subdocument
	int ccpTxbx = OleUtil::get4Bytes(buffer, 0x0064); //textbox subdocument
	int ccpHdrTxbx = OleUtil::get4Bytes(buffer, 0x0068); //textbox subdocument of the header
	int lastCP = ccpFtn + ccpHdd + ccpMcr + ccpAtn + ccpEdn + ccpTxbx + ccpHdrTxbx;
	if (lastCP != 0) {
		++lastCP;
	}
	lastCP += ccpText;
	return lastCP;
}

std::string OleMainStream::getPiecesTableBuffer(const char* headerBuffer, OleStream& tableStream) {
	unsigned int clxOffset = OleUtil::getU4Bytes(headerBuffer, 0x01A2); //offset for CLX structure
	unsigned int clxLength = OleUtil::getU4Bytes(headerBuffer, 0x01A6); //offset for value of CLX structure length

	//1 step : loading CLX table from table stream
	char* clxBuffer = new char[clxLength];
	tableStream.seek(clxOffset, true);
	tableStream.read(clxBuffer, clxLength);
	std::string clx(clxBuffer, clxLength);
	delete clxBuffer;

	//2 step: searching for pieces table buffer at CLX
	//(determines it by 0x02 as start symbol)
	size_t from = 0;
	size_t i;
	std::string pieceTableBuffer;
	while ((i = clx.find_first_of(0x02, from)) != std::string::npos) {
		unsigned int pieceTableLength = OleUtil::getU4Bytes(clx.c_str(), i + 1);
		pieceTableBuffer = std::string(clx, i + 1 + 4);
		if (pieceTableBuffer.length() != pieceTableLength) {
			from = i + 1;
			continue;
		}
		break;
	}
	return pieceTableBuffer;
}


bool OleMainStream::readPieceTable(const char* headerBuffer, const OleEntry& tableEntry) {
	OleStream tableStream(myStorage, tableEntry, myBaseStream);
	std::string piecesTableBuffer = getPiecesTableBuffer(headerBuffer, tableStream);

	//getting the CP (character positions) and CP descriptors
	int lastCP = getLastCP(headerBuffer);
	std::vector<int> cp; //array of charachter positions for pieces
	unsigned int j = 0;
	for (j = 0; ; j += 4) {
		int curCP = OleUtil::get4Bytes(piecesTableBuffer.c_str(), j);
		cp.push_back(curCP);
		if (curCP == lastCP) {
			break;
		}
	}
	std::vector<std::string> descriptors;
	for (size_t k = 0; k < cp.size() - 1; ++k) {
		//j + 4, because it should be taken after CP in PiecesTable Buffer
		//k * 8, because it should be taken 8 byte for each descriptor
		descriptors.push_back(piecesTableBuffer.substr(j + 4 + k * 8, 8));
	}

	//filling the Pieces vector
	for (size_t i = 0; i < descriptors.size(); ++i) {
		//4byte integer with offset and ANSI flag
		int fcValue = OleUtil::get4Bytes(descriptors.at(i).c_str(), 0x2); //offset for piece structure
		Piece piece;
		piece.isANSI = (fcValue & 0x40000000) == 0x40000000; //ansi flag
		piece.offset = fcValue & 0x3FFFFFFF; //gettting offset for current piece
		piece.length = cp[i + 1] - cp[i];
		if (!piece.isANSI) {
			piece.length *= 2;
		} else {
			piece.offset /= 2;
		}
		myPieces.push_back(piece);
	}
	return true;
}

bool OleMainStream::readStylesheet(const char *headerBuffer, const OleEntry &tableEntry) {
	unsigned int beginStshInfo = OleUtil::getU4Bytes(headerBuffer, 0xa2); // address of STSH structure
	size_t stshInfoLen = (size_t)OleUtil::getU4Bytes(headerBuffer, 0xa6); // length of STSH structure

	OleStream tableStream(myStorage, tableEntry, myBaseStream);
	char* buffer = new char[stshInfoLen];
	tableStream.seek(beginStshInfo, true);
	if (tableStream.read(buffer, stshInfoLen) != stshInfoLen) {
		return false;
	}

	size_t stdCount = (size_t)OleUtil::getU2Bytes(buffer, 2);
	size_t stdBaseInFile = (size_t)OleUtil::getU2Bytes(buffer, 4);
	myStyleSheetStyleInfo.resize(stdCount);
	myStyleSheetCharInfo.resize(stdCount);

	std::vector<bool> isFilled;
	isFilled.resize(stdCount, false);

	size_t stdLen = 0;
	bool styleSheetWasChanged = false;
	do { //make it in while loop, because some base style can be after their successors
		styleSheetWasChanged = false;
		for (size_t index = 0, offset = 2 + (size_t)OleUtil::getU2Bytes(buffer, 0); index < stdCount; index++, offset += 2 + stdLen) {
			stdLen = (size_t)OleUtil::getU2Bytes(buffer, offset);
			if (isFilled.at(index)) {
				continue;
			}

			if (stdLen == 0) {
				//if record is empty, left it default
				isFilled[index] = true;
				continue;
			}

			StyleInfo styleInfo = myStyleSheetStyleInfo.at(index);
			CharInfo charInfo = myStyleSheetCharInfo.at(index);

			unsigned int styleAndBaseType = OleUtil::getU2Bytes(buffer, offset + 4);
			unsigned int styleType = styleAndBaseType % 16;
			unsigned int baseStyle = styleAndBaseType / 16;
			if (baseStyle == STI_NIL || baseStyle == STI_USER) {
				//if based on nil or user style, left defaukt
			} else {
				int baseStyleIndex = getStyleIndex(baseStyle, isFilled, myStyleSheetStyleInfo);
				if (baseStyleIndex < 0) {
					//this base style is not filled yet, sp pass it at some time
					continue;
				}
				styleInfo = myStyleSheetStyleInfo.at(baseStyleIndex);
				styleInfo.istd = ISTD_INVALID;
				charInfo = myStyleSheetCharInfo.at(baseStyleIndex);
			}

			// parse STD structure
			unsigned int tmp = OleUtil::getU2Bytes(buffer, offset + 6);
			unsigned int upxCount = tmp % 16;
			styleInfo.istdNext = tmp / 16;

			//adding current styles & char info
			myStyleSheetStyleInfo[index] = styleInfo;
			myStyleSheetCharInfo[index] = charInfo;
			isFilled[index] = true;
			styleSheetWasChanged = true;

			size_t pos = 2 + stdBaseInFile;
			size_t nameLen = (size_t)OleUtil::getU2Bytes(buffer, offset + pos);
			nameLen = nameLen * 2 + 2; //from Unicode characters to bytes + Unicode null charachter length
			pos += 2 + nameLen;
			if (pos % 2 != 0) {
				++pos;
			}
			if (pos >= stdLen) {
				continue;
			}
			size_t upxLen = (size_t)OleUtil::getU2Bytes(buffer, offset + pos);
			if (pos + upxLen > stdLen) {
				//UPX length too large
				continue;
			}
			//for style info styleType must be equal 1
			if (styleType == 1 && upxCount >= 1) {
				if (upxLen >= 2) {
					styleInfo.istd = OleUtil::getU2Bytes(buffer, offset + pos + 2);
					getStyleInfo(0, buffer + offset + pos + 4, upxLen - 2, styleInfo);
					myStyleSheetStyleInfo[index] = styleInfo;
				}
				pos += 2 + upxLen;
				if (pos % 2 != 0) {
					++pos;
				}
				upxLen = (size_t)OleUtil::getU2Bytes(buffer, offset + pos);
			}
			if (upxLen == 0 || pos + upxLen > stdLen) {
				//too small/too large
				continue;
			}
			//for char info styleType can be equal 1 or 2
			if ((styleType == 1 && upxCount >= 2) || (styleType == 2 && upxCount >= 1)) {
				getCharInfo(0, ISTD_INVALID, buffer + offset + pos + 2, upxLen, charInfo);
				myStyleSheetCharInfo[index] = charInfo;
			}
		}
	} while (styleSheetWasChanged);
	delete buffer;
	return true;
}

bool OleMainStream::readCharInfoTable(const char* headerBuffer, const OleEntry& tableEntry) {
	unsigned int beginCharInfo = OleUtil::getU4Bytes(headerBuffer, 0xfa); // address of fcPlcfbteChpx structure
	size_t charInfoLen = (size_t)OleUtil::getU4Bytes(headerBuffer, 0xfe); // length of fcPlcfbteChpx structure
	if (charInfoLen < 4) {
		return false;
	}

	OleStream tableStream(myStorage, tableEntry, myBaseStream);
	char* buffer = new char[charInfoLen];
	tableStream.seek(beginCharInfo, true);
	if (tableStream.read(buffer, charInfoLen) != charInfoLen) {
		return false;
	}
	std::string chpx(buffer, charInfoLen);
	delete buffer;

	size_t size = (charInfoLen / 4 - 1) / 2;
	std::vector<unsigned int> charPage;
	for (size_t index = 0, offset = (size + 1) * 4; index < size; ++index, offset += 4) {
		charPage.push_back(OleUtil::getU4Bytes(chpx.c_str(), offset));
	}

	char* formatPageBuffer = new char[OleStorage::BBD_BLOCK_SIZE];
	for (size_t index = 0; index < size; ++index) {
		seek(charPage.at(index) * OleStorage::BBD_BLOCK_SIZE, true);
		if (read(formatPageBuffer, OleStorage::BBD_BLOCK_SIZE) != OleStorage::BBD_BLOCK_SIZE) {
			return false;
		}
		unsigned int crun = OleUtil::getU1Byte(formatPageBuffer, 0x1ff); //offset with crun (count of 'run of text')
		for (unsigned int index2 = 0; index2 < crun; ++index2) {
			unsigned int charOffset = OleUtil::getU4Bytes(formatPageBuffer, index2 * 4);
			unsigned int chpxOffset = 2 * OleUtil::getU1Byte(formatPageBuffer, (crun + 1) * 4 + index2);
			unsigned int len = OleUtil::getU1Byte(formatPageBuffer, chpxOffset);
			unsigned int istd = getIstdByOffset(charOffset, myStyleInfoList);

			CharInfo charInfo = getCharInfoFromStylesheet(istd, myStyleSheetStyleInfo, myStyleSheetCharInfo);
			charInfo.offset = charOffset;
			if (chpxOffset != 0) {
				getCharInfo(chpxOffset, istd, formatPageBuffer + 1, len - 1, charInfo);
			}
			myCharInfoList.push_back(charInfo);
		}
	}
	delete formatPageBuffer;
	return true;
}

bool OleMainStream::readParagraphStyleTable(const char *headerBuffer, const OleEntry &tableEntry) {
	unsigned int beginParfInfo = OleUtil::getU4Bytes(headerBuffer, 0x102); // address of PlcBtePapx structure
	size_t parfInfoLen = (size_t)OleUtil::getU4Bytes(headerBuffer, 0x106); // length of PlcBtePapx structure
	if (parfInfoLen < 4) {
		return false;
	}

	OleStream tableStream(myStorage, tableEntry, myBaseStream);
	char* buffer = new char[parfInfoLen];
	tableStream.seek(beginParfInfo, true);
	if (tableStream.read(buffer, parfInfoLen) != parfInfoLen) {
		return false;
	}
	std::string papx(buffer, parfInfoLen);
	delete buffer;

	size_t size = (parfInfoLen / 4 - 1) / 2;

	std::vector<unsigned int> paragraphPages;
	for (size_t index = 0, tOffset = (size + 1) * 4; index < size; ++index, tOffset += 4) {
		paragraphPages.push_back(OleUtil::getU4Bytes(papx.c_str(), tOffset));
	}

	char* formatPageBuffer = new char[OleStorage::BBD_BLOCK_SIZE];
	for (size_t index = 0; index < size; ++index) {
		seek(paragraphPages.at(index) * OleStorage::BBD_BLOCK_SIZE, true);
		if (read(formatPageBuffer, OleStorage::BBD_BLOCK_SIZE) != OleStorage::BBD_BLOCK_SIZE) {
			return false;
		}
		unsigned int cpara = OleUtil::getU1Byte(formatPageBuffer, 0x1ff); //offset with cpara (count of paragraphs)
		for (unsigned int index2 = 0; index2 < cpara; ++index2) {
			unsigned int charPos = OleUtil::getU4Bytes(formatPageBuffer, index2 * 4);
			unsigned int papxOffset = OleUtil::getU1Byte(formatPageBuffer, (cpara + 1) * 4 + index2 * 13) * 2;
			if (papxOffset <= 0) {
				continue;
			}
			unsigned int len = OleUtil::getU1Byte(formatPageBuffer, papxOffset) * 2;
			if (len == 0) {
				++papxOffset;
				len = OleUtil::getU1Byte(formatPageBuffer, papxOffset) * 2;
			}

			unsigned int istd = OleUtil::getU2Bytes(formatPageBuffer, papxOffset + 1);
			StyleInfo styleInfo = getStyleInfoFromStylesheet(istd, myStyleSheetStyleInfo);
			styleInfo.offset = charPos;
			CharInfo charInfo = getCharInfoFromStylesheet(istd, myStyleSheetStyleInfo, myStyleSheetCharInfo);
			styleInfo.fontStyle = charInfo.fontStyle;
			styleInfo.fontSize = charInfo.fontSize;

			if (len >= 3) {
				getStyleInfo(papxOffset, formatPageBuffer + 3, len - 3, styleInfo);
			}
			myStyleInfoList.push_back(styleInfo);
		}
	}
	delete formatPageBuffer;
	return true;
}

bool OleMainStream::readSectionsInfoTable(const char *headerBuffer, const OleEntry &tableEntry) {
	unsigned int beginOfText = OleUtil::getU4Bytes(headerBuffer, 0x18); //Min
	unsigned int beginSectInfo = OleUtil::getU4Bytes(headerBuffer, 0xca); //address if PlcfSed structure

	size_t sectInfoLen = (size_t)OleUtil::getU4Bytes(headerBuffer, 0xce); //length of PlcfSed structure
	if (sectInfoLen < 4) {
		return false;
	}

	OleStream tableStream(myStorage, tableEntry, myBaseStream);
	char* buffer = new char[sectInfoLen];
	tableStream.seek(beginSectInfo, true);
	if (tableStream.read(buffer, sectInfoLen) != sectInfoLen) {
		return false;
	}
	std::string sepx(buffer, sectInfoLen);
	delete buffer;

	size_t decriptorsCount = (sectInfoLen - 4) / 16;

	//saving the section offsets (in character positions)
	std::vector<unsigned int> charPos;
	for (size_t index = 0, tOffset = 0; index < decriptorsCount; ++index, tOffset += 4) {
		unsigned int ulTextOffset = OleUtil::getU4Bytes(sepx.c_str(), tOffset);
		charPos.push_back(beginOfText + ulTextOffset);
	}

	//saving sepx offsets
	std::vector<unsigned int> sectPage;
	for (size_t index = 0, tOffset = (decriptorsCount + 1) * 4; index < decriptorsCount; ++index, tOffset += 12) {
		sectPage.push_back(OleUtil::getU4Bytes(sepx.c_str(), tOffset + 2));
	}

	//reading the section properties
	char tmpBuffer[2];
	for (size_t index = 0; index < decriptorsCount; ++index) {
		if (sectPage.at(index) == 0xffffffffUL) { //check for invalid record, to make default section info
			SectionInfo sectionInfo;
			sectionInfo.charPos = charPos.at(index);
			mySectionInfoList.push_back(sectionInfo);
			continue;
		}
		//getting number of bytes to read
		seek(sectPage.at(index), true);
		if (read(tmpBuffer, 2) != 2) {
			return false;
		}
		size_t bytes = 2 + (size_t)OleUtil::getU2Bytes(tmpBuffer, 0);

		char* formatPageBuffer = new char[bytes];
		seek(sectPage.at(index), true);
		if (read(formatPageBuffer, bytes) != bytes) {
			delete formatPageBuffer;
			continue;
		}
		SectionInfo sectionInfo;
		sectionInfo.charPos = charPos.at(index);
		getSectionInfo(formatPageBuffer + 2, bytes - 2, sectionInfo);
		mySectionInfoList.push_back(sectionInfo);
		delete formatPageBuffer;
	}
	return true;
}

void OleMainStream::getStyleInfo(unsigned int papxOffset, const char *grpprlBuffer, unsigned int bytes, StyleInfo& styleInfo) {
	int	tmp, del, add;
	unsigned int offset = 0;
	while (bytes >= offset + 2) {
		unsigned int curInfoLength = 0;
		switch (OleUtil::getU2Bytes(grpprlBuffer, papxOffset + offset)) {
			case 0x2403:
				styleInfo.alignment = OleUtil::getU1Byte(grpprlBuffer, papxOffset + offset + 2);
				break;
			case 0x260a:
				styleInfo.listLevel =	OleUtil::getU1Byte(grpprlBuffer, papxOffset + offset + 2);
				styleInfo.numLevel = styleInfo.listLevel;
				break;
			case 0x4600: //istd
				//tmp = OleUtil::getU2Bytes(grpprlBuffer, papxOffset + offset + 2);
				break;
			case 0x460b:
				styleInfo.listIndex =	OleUtil::getU2Bytes(grpprlBuffer, papxOffset + offset + 2);
				break;
			case 0x4610:
				styleInfo.leftIndent += OleUtil::getU2Bytes(grpprlBuffer, papxOffset + offset + 2);
				if (styleInfo.leftIndent < 0) {
					styleInfo.leftIndent = 0;
				}
				break;
			case 0xc60d: // ChgTabsPapx
			case 0xc615: // ChgTabs
				tmp = OleUtil::get1Byte(grpprlBuffer, papxOffset + offset + 2);
				if (tmp < 2) {
					curInfoLength = 1;
					break;
				}
				del = OleUtil::getU1Byte(grpprlBuffer, papxOffset + offset + 3);
				if (tmp < 2 + 2 * del) {
					curInfoLength = 1;
					break;
				}
				add = OleUtil::getU1Byte(grpprlBuffer, papxOffset + offset + 4 + 2 * del);
				if (tmp < 2 + 2 * del + 2 * add) {
					curInfoLength = 1;
					break;
				}
				break;
			case 0x840e:
				styleInfo.rightIndent = (int)OleUtil::getU2Bytes(grpprlBuffer, papxOffset + offset + 2);
				break;
			case 0x840f:
				styleInfo.leftIndent = (int)OleUtil::getU2Bytes(grpprlBuffer, papxOffset + offset + 2);
				break;
			case 0x8411:
				styleInfo.leftIndent1 = (int)OleUtil::getU2Bytes(grpprlBuffer, papxOffset + offset + 2);
				break;
			case 0xa413:
				styleInfo.beforeIndent = OleUtil::getU2Bytes(grpprlBuffer, papxOffset + offset + 2);
				break;
			case 0xa414:
				styleInfo.afterIndent = OleUtil::getU2Bytes(grpprlBuffer, papxOffset + offset + 2);
				break;
			default:
				break;
		}
		if (curInfoLength == 0) {
			curInfoLength = getInfoLength(grpprlBuffer, papxOffset + offset);
		}
		offset += curInfoLength;
	}

}

void OleMainStream::getCharInfo(unsigned int chpxOffset, unsigned int /*istd*/, const char *grpprlBuffer, unsigned int iBytes, CharInfo& charInfo) {

	unsigned int tmp = 0;
	unsigned int offset = 0;

	while (iBytes >= offset + 2) {
		switch (OleUtil::getU2Bytes(grpprlBuffer, chpxOffset + offset)) {
			case 0x0835: //bold
				tmp = OleUtil::getU1Byte(grpprlBuffer, chpxOffset + offset + 2);
				switch (tmp) {
					case UNSET:
						charInfo.fontStyle &= ~CharInfo::BOLD;
						break;
					case SET:
						charInfo.fontStyle |= CharInfo::BOLD;
						break;
					case UNCHANGED:
						break;
					case NEGATION:
						charInfo.fontStyle ^= CharInfo::BOLD;
						break;
					default:
						break;
				}
				break;
			case 0x0836: //italic
				tmp = OleUtil::getU1Byte(grpprlBuffer, chpxOffset + offset + 2);
				switch (tmp) {
					case UNSET:
						charInfo.fontStyle &= ~CharInfo::ITALIC;
						break;
					case SET:
						charInfo.fontStyle |= CharInfo::ITALIC;
						break;
					case UNCHANGED:
						break;
					case NEGATION:
						charInfo.fontStyle ^= CharInfo::ITALIC;
						break;
					default:
						break;
					}
				break;
			case 0x4a43: //size of font
				charInfo.fontSize = OleUtil::getU2Bytes(grpprlBuffer, chpxOffset + offset + 2);
			break;
			default:
				break;
		}
		offset += getInfoLength(grpprlBuffer, chpxOffset + offset);
	}

}

void OleMainStream::getSectionInfo(const char* grpprlBuffer, size_t bytes, SectionInfo& sectionInfo) {
	unsigned int tmp;

	size_t offset = 0;
	while (bytes >= offset + 2) {
		switch (OleUtil::getU2Bytes(grpprlBuffer, offset)) {
			case 0x3009: //new page
				tmp = OleUtil::getU1Byte(grpprlBuffer, offset + 2);
				sectionInfo.newPage = tmp != 0 && tmp != 1;
				break;
			default:
				break;
		}
		offset += getInfoLength(grpprlBuffer, offset);
	}
}

OleMainStream::StyleInfo OleMainStream::getStyleInfoFromStylesheet(unsigned int istd, const OleMainStream::StyleInfoList &styleInfoList) {
	StyleInfo styleInfo;
	if (istd != ISTD_INVALID && istd != STI_NIL && istd != STI_USER) {
		for (size_t index = 0; index < styleInfoList.size(); ++index) {
			if (styleInfoList.at(index).istd == istd) {
				return styleInfoList.at(index);
			}
		}
	}
	styleInfo.istd = istd;
	return styleInfo;
}

OleMainStream::CharInfo OleMainStream::getCharInfoFromStylesheet(unsigned int istd, const OleMainStream::StyleInfoList &styleInfoList, const OleMainStream::CharInfoList &charInfoList) {
	CharInfo charInfo;
	if (istd != ISTD_INVALID && istd != STI_NIL && istd != STI_USER) {
		for (size_t index = 0; index < styleInfoList.size(); ++index) {
			if (styleInfoList.at(index).istd == istd) {
				return charInfoList.at(index);
			}
		}
	}
	return charInfo;
}

int OleMainStream::getStyleIndex(unsigned int istd, const std::vector<bool>& isFilled, const StyleInfoList& styleInfoList) {
	if (istd == ISTD_INVALID) {
		return -1;
	}
	for (int index = 0; index < (int)styleInfoList.size(); ++index) {
		if (isFilled.at(index) && styleInfoList.at(index).istd == istd) {
			return index;
		}
	}
	return -1;
}

unsigned int OleMainStream::getIstdByOffset(unsigned int offset, const OleMainStream::StyleInfoList &styleInfoList) {
	unsigned int istd = ISTD_INVALID;
	for (size_t i = 0; i < styleInfoList.size(); ++i) {
		const StyleInfo& info = styleInfoList.at(i);
		if (i == styleInfoList.size() - 1) { //if last
			istd = info.istd;
			break;
		}
		const StyleInfo& infoNext = styleInfoList.at(i + 1);
		if (offset >= info.offset && offset < infoNext.offset) {
			istd = info.istd;
			break;
		}
	}
	return istd;
}

unsigned int OleMainStream::getInfoLength(const char *grpprlBuffer, unsigned int byteNumber) {
	unsigned int tmp;

	unsigned int opCode = OleUtil::getU2Bytes(grpprlBuffer, byteNumber);
	switch (opCode & 0xe000) {
		case 0x0000:
		case 0x2000:
			return 3;
		case 0x4000:
		case 0x8000:
		case 0xA000:
			return 4;
		case 0xE000:
			return 5;
		case 0x6000:
			return 6;
		case 0xC000:
			//counting of info length
			tmp = OleUtil::getU1Byte(grpprlBuffer, byteNumber + 2);
			if (opCode == 0xc615 && tmp == 255) {
				unsigned int del = OleUtil::getU1Byte(grpprlBuffer, byteNumber + 3);
				unsigned int add = OleUtil::getU1Byte(grpprlBuffer, byteNumber + 4 + del * 4);
				tmp = 2 + del * 4 + add * 3;
			}
			return 3 + tmp;
		default:
			return 1;
	}
}
