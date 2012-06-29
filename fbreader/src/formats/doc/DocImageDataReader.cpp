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

#include "OleUtil.h"

#include "DocImageDataReader.h"

DocImageDataReader::DocImageDataReader(shared_ptr<OleStream> dataStream) :
	myDataStream(dataStream) {
}

OleStream::BlockPieceInfoList DocImageDataReader::getImagePieceInfo(unsigned int dataPos) {
	if (myDataStream.isNull()) {
		printf("data stream is null\n");
		return OleStream::BlockPieceInfoList();
	}
	if (!myDataStream->seek(dataPos, true)) {
		return OleStream::BlockPieceInfoList();
	}

	size_t dataPosFileOffset = myDataStream->fileOffset();

	//reading PICF structure
	unsigned int picfHeaderSize = 4 + 2 + 8; //record length, headerLength and storage format
	char headerBuffer[picfHeaderSize];
	if (myDataStream->read(headerBuffer, picfHeaderSize) != picfHeaderSize) {
		return OleStream::BlockPieceInfoList();
	}
	unsigned int length = OleUtil::getU4Bytes(headerBuffer, 0);
	unsigned int headerLength = OleUtil::getU2Bytes(headerBuffer, 4);
	unsigned int formatType = OleUtil::getU2Bytes(headerBuffer, 6);

	if (formatType != 0x0064) { //external link to some file; see p.394 [MS-DOC]
		//TODO implement
		return OleStream::BlockPieceInfoList();
	}
	if (headerLength >= length) {
		return OleStream::BlockPieceInfoList();
	}

	//reading OfficeArtInlineSpContainer structure; see p.421 [MS-DOC] and p.56 [MS-ODRAW]
	if (!myDataStream->seek(headerLength - picfHeaderSize, false)) {  //skip header
		return OleStream::BlockPieceInfoList();
	}

	char buffer[8]; //for OfficeArtRecordHeader structure; see p.69 [MS-ODRAW]
	bool found = false;
	unsigned int curOffset = 0;
	for (curOffset = headerLength; !found && curOffset + 8 <= length; curOffset += 8) {
		if (myDataStream->read(buffer, 8) != 8) {
			return OleStream::BlockPieceInfoList();
		}
		unsigned int recordInstance = OleUtil::getU2Bytes(buffer, 0) >> 4;
		unsigned int recordType = OleUtil::getU2Bytes(buffer, 2);
		unsigned int recordLen = OleUtil::getU4Bytes(buffer, 4);

		printf("RecordType: 0x%X (%u)\n", recordType, recordLen);

		switch (recordType) {
			case 0xF000: case 0xF001: case 0xF002: case 0xF003: case 0xF004: case 0xF005:
				break;
			case 0xF007:
				{
					myDataStream->seek(33, false);
					char tmpBuf[1];
					myDataStream->read(tmpBuf, 1);
					unsigned int nameLength = OleUtil::getU1Byte(tmpBuf, 0);
					myDataStream->seek(nameLength * 2 + 2, false);
					curOffset += 33 + 1 + nameLength * 2 + 2;
				}
				break;
			case 0xF008:
				myDataStream->seek(8, false);
				curOffset += 8;
				break;
			case 0xF009:
				myDataStream->seek(16, false);
				curOffset += 16;
				break;
			case 0xF006: case 0xF00A: case 0xF00B: case 0xF00D: case 0xF00E: case 0xF00F: case 0xF010: case 0xF011: case 0xF122:
				myDataStream->seek(recordLen, false);
				curOffset += recordLen;
				break;
			case 0xF01A: //EMF
			case 0xF01B: //WMF
			case 0xF01C: //PICT
				//TODO implement
				return OleStream::BlockPieceInfoList();
			case 0xF01D: //JPEG
				myDataStream->seek(17, false);
				curOffset += 17;
				if (recordInstance == 0x46B || recordInstance == 0x6E3) {
					myDataStream->seek(16, false);
					curOffset += 16;
				}
				found = true;
				break;
			case 0xF01E: //PNG
				myDataStream->seek(17, false);
				curOffset += 17;
				if (recordInstance == 0x6E1) {
					myDataStream->seek(16, false);
					curOffset += 16;
				}
				found = true;
				break;
			case 0xF01F: //DIB (BMP without 14-bytes header)
				myDataStream->seek(17, false);
				curOffset += 17;
				if (recordInstance == 0x7A9) {
					myDataStream->seek(16, false);
					curOffset += 16;
				}
				found = true;
				break;
			case 0xF020: //TIFF
				myDataStream->seek(17, false);
				curOffset += 17;
				if (recordInstance == 0x6E5) {
					myDataStream->seek(16, false);
					curOffset += 16;
				}
				found = true;
				break;
			case 0xF00C:
			default:
				return OleStream::BlockPieceInfoList();
			}
	}

	if (!found) {
		printf("\n!found\n");
		return OleStream::BlockPieceInfoList();
	}

	printf("\nfound!!!!\n");
	OleStream::BlockPieceInfoList list2 = myDataStream->getBlockPieceInfoList(dataPos + curOffset, length - curOffset);
	OleStream::BlockPieceInfoList list;
	OleStream::BlockPieceInfo info(dataPosFileOffset + curOffset, length - curOffset);
	list.push_back(info);

	printf("one big piece: off=%u, size=%u\n", info.offset, info.size);
	for (size_t i = 0; i < list2.size(); ++i) {
		printf("piece[%u] = off=%u, size=%u, next=%u\n", i, list2.at(i).offset, list2.at(i).size,  list2.at(i).offset + list2.at(i).size);
	}
	return list;
}
