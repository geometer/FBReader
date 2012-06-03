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

#include <ZLLogger.h>
#include <ZLUnicodeUtil.h>

#include "OleUtil.h"
#include "OleStorage.h"

#include "OleMainStream.h"

static const int FLAG_DOT = 0x0001;
static const int FLAG_GLSY = 0x0002;
static const int FLAG_COMPLEX = 0x0004;
static const int FLAG_PICTURES = 0x0008;
static const int FLAG_ENCRYPTED = 0x100;
static const int FLAG_READONLY = 0x400;
static const int FLAG_RESERVED = 0x800;
static const int FLAG_EXTCHAR = 0x1000;

static const int FLAGS_OFFSET = 0xA;

static const int FILEINFO_BLOCK_VERSION = 0x2;
static const int PRODUCT_VERSION = 0x4;
static const int LANGUAGE = 0x6;

static const int CHARSET = 0x14;
static const unsigned int DEFAULT_CHARSET = 0x100;

static const int START_OF_TEXT = 0x18;
static const int END_OF_TEXT = 0x1c;

static const size_t FIB_SIZE = 128;
static const size_t EXTENDED_HEADER_SIZE = 512;

static const unsigned int TABLE_NUMBER_FLAG = 0x0200;

//following constants were getted from p. 56-57 from official documentation ([MS-DOC])
static const unsigned int CCP_TEXT_OFFSET = 0x004C;
static const unsigned int CCP_FTN_OFFSET = 0x0050;
static const unsigned int CCP_HDD_OFFSET = 0x0054;
static const unsigned int CCP_MCR_OFFSET = 0x0058;
static const unsigned int CCP_ATN_OFFSET = 0x005C;
static const unsigned int CCP_EDN_OFFSET = 0x0060;
static const unsigned int CCP_TXBX_OFFSET = 0x0064;
static const unsigned int CCP_HDR_TXBX_OFFSET = 0x0068;

static const unsigned int FC_CLX_OFFSET = 0x01A2;
static const unsigned int LCB_CLX_OFFSET = 0x01A6;

const static long PIECE_ANSI_FLAG = 0x40000000;
const static long PIECE_OFFSET_FLAG = 0x3FFFFFFF;
const static unsigned int PIECE_VALUE_OFFSET = 0x2;

static const char START_CHAR_OF_PIECE_TABLE = 0x02;

OleMainStream::OleMainStream(shared_ptr<OleStorage> storage, OleEntry oleEntry, shared_ptr<ZLInputStream> stream) :
	OleStream(storage, oleEntry, stream) {
}

bool OleMainStream::open() {
	if (OleStream::open() == false) {
		return false;
	}
	return readFIB() && readPieceTable();
}

const OleMainStream::Pieces& OleMainStream::getPieces() const {
	return myPieces;
}

bool OleMainStream::readFIB() {
	char header[FIB_SIZE];
	seek(0, true);
	if (read(header, FIB_SIZE) != FIB_SIZE) {
		ZLLogger::Instance().println("OleMainStream", "Can't read FIB from WordDocument stream");
		return false;
	}

	int flags = OleUtil::getUShort(header, FLAGS_OFFSET);

	if (flags & FLAG_COMPLEX) {
		ZLLogger::Instance().println("OleMainStream", "This was fast-saved. Some information is lost");
		//lostInfo = (flags & 0xF0) >> 4);
	}

	if (flags & FLAG_EXTCHAR) {
		ZLLogger::Instance().println("OleMainStream", "File uses extended character set (get_word8_char)");
	} else {
		ZLLogger::Instance().println("OleMainStream", "File uses get_8bit_char character set");
	}

	if (flags & FLAG_ENCRYPTED) {
		ZLLogger::Instance().println("OleMainStream", "File is encrypted");
		// Encryption key = %08lx ; NumUtil::getlong(header, 14)
		return false;
	}

	unsigned int charset = OleUtil::getUShort(header, CHARSET);
	if (charset && charset != DEFAULT_CHARSET) {
		ZLLogger::Instance().println("OleMainStream", "Using not default character set %d");
	} else {
		ZLLogger::Instance().println("OleMainStream", "Using default character set");
	}

	myStartOfText = OleUtil::getLong(header, START_OF_TEXT);
	myEndOfText = OleUtil::getLong(header, END_OF_TEXT);
	//printf("length of text = %lu\n", myEndOfText - myStartOfText);
	return true;
}

long OleMainStream::getLastCP(char* buffer) {
	long ccpText = OleUtil::getLong(buffer, CCP_TEXT_OFFSET);
	long ccpFtn = OleUtil::getLong(buffer, CCP_FTN_OFFSET);
	long ccpHdd = OleUtil::getLong(buffer, CCP_HDD_OFFSET);
	long ccpMcr = OleUtil::getLong(buffer, CCP_MCR_OFFSET);
	long ccpAtn = OleUtil::getLong(buffer, CCP_ATN_OFFSET);
	long ccpEdn = OleUtil::getLong(buffer, CCP_EDN_OFFSET);
	long ccpTxbx = OleUtil::getLong(buffer, CCP_TXBX_OFFSET);
	long ccpHdrTxbx = OleUtil::getLong(buffer, CCP_HDR_TXBX_OFFSET);
	long lastCP = ccpFtn + ccpHdd + ccpMcr + ccpAtn + ccpEdn + ccpTxbx + ccpHdrTxbx;
	if (lastCP != 0) {
		++lastCP;
	}
	lastCP += ccpText;
//	printf("ccpText = %ld, ccpFtn = %ld, ccpHdd = %ld, ccpMcr = %ld\n", ccpText, ccpFtn, ccpHdd, ccpMcr);
//	printf("ccpAtn = %ld, ccpEdn = %ld, ccpTxbx = %ld, ccpHdrTxbx = %ld, lastCP = %ld\n", ccpAtn, ccpEdn, ccpTxbx, ccpHdrTxbx, lastCP);
	return lastCP;
}

std::string OleMainStream::getPiecesTableBuffer(char* headerBuffer, OleStream& tableStream) {
	unsigned long clxOffset = OleUtil::getULong(headerBuffer, FC_CLX_OFFSET);
	unsigned long clxLength = OleUtil::getULong(headerBuffer, LCB_CLX_OFFSET);
	//printf("fcClx = %lu, lcbClx = %lu\n", clxOffset, clxLength);

	//1 step : loading CLX table from table stream
	char* clxBuffer = new char[clxLength];
	tableStream.seek(clxOffset, true);
	tableStream.read(clxBuffer, clxLength);
	std::string clx(clxBuffer, clxLength);
	delete clxBuffer;

	//2 step: searching for pieces table buffer at CLX
	//(determines it by START_CHAR_OF_PIECE_TABLE as start symbol)
	size_t from = 0;
	size_t i;
	std::string pieceTableBuffer;
	while ((i = clx.find_first_of(START_CHAR_OF_PIECE_TABLE, from)) != std::string::npos) {
		unsigned long pieceTableLength = OleUtil::getULong(clx.c_str(), i + 1);
		pieceTableBuffer = std::string(clx, i + 1 + 4);
		if (pieceTableBuffer.length() != pieceTableLength) {
			from = i + 1;
			continue;
		}
		//printf("lcbPieceTable = %lu, from = %lu, i = %lu \n", pieceTableLength, from, i);
		break;
	}
	return pieceTableBuffer;
}


bool OleMainStream::readPieceTable() {
	char headerBuffer[EXTENDED_HEADER_SIZE];
	seek(0, true);
	read(headerBuffer, EXTENDED_HEADER_SIZE);
	unsigned short tableNumber = (OleUtil::getUShort(headerBuffer, FLAGS_OFFSET) & TABLE_NUMBER_FLAG) ? 1 : 0;
	//printf("myTableNumber = %d\n", tableNumber);
	std::string tableName = tableNumber == 0 ? "0" : "1";
	tableName += "Table";
	OleEntry tableEntry;
	bool result = myStorage->getEntryByName(tableName, tableEntry);
	if (!result) {
		//cant't find table stream, so building simple table stream
		Piece piece = {myStartOfText, myEndOfText - myStartOfText, true}; //CHECK may be not all old documents have ANSI
		myPieces.push_back(piece);
		return true;
	}
	//printf("Table Name: %s\n", tableName.c_str());

	OleStream tableStream(myStorage, tableEntry, myBaseStream);
	std::string piecesTableBuffer = getPiecesTableBuffer(headerBuffer, tableStream);

	//getting the CP (character positions) and CP descriptors
	long lastCP = getLastCP(headerBuffer);
	std::vector<long> cp; //array of charachter positions for pieces
	unsigned int j = 0;
	for (j = 0; ; j += 4) {
		long curCP = OleUtil::getLong(piecesTableBuffer.c_str(), j);
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

	//for (size_t i = 0; i < piecesStartPositions.size(); ++i) {
	//	printf("myPieceTable[%lu] = %ld\n", i, piecesStartPositions.at(i));
	//}
	//printf("count of descriptors = %lu\n", piecesDescriptors.size());
	//for (size_t i = 0; i < piecesDescriptors.size(); ++i) {
	//	for (size_t j = 0; j < 8; ++j) {
	//		printf("%d ", piecesDescriptors.at(i)[j]);
	//	}
	//	printf("\n");
	//}

	//filling the Pieces vector
	for (size_t i = 0; i < descriptors.size(); ++i) {
		//long integer with offset and ANSI flag
		long fcValue = OleUtil::getLong(descriptors.at(i).c_str(), PIECE_VALUE_OFFSET);
		Piece piece;
		piece.isANSI = (fcValue & PIECE_ANSI_FLAG) == PIECE_ANSI_FLAG;
		piece.offset = fcValue & PIECE_OFFSET_FLAG;
		piece.length = cp[i + 1] - cp[i];
		if (!piece.isANSI) {
			piece.length *= 2;
		} else {
			piece.offset /= 2;
		}
		myPieces.push_back(piece);
		//printf("\n\n\n");
		//printf("fcValue = %ld, isAnsi = %s, fc = %ld, lcb = %ld, total = %ld\n", fcValue, piece.isANSI ? "true" : "false", piece.offset, piece.length, piece.offset + piece.length);
	}
	return true;
}
