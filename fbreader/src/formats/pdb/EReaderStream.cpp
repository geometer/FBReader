/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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
#include <cctype>

#include <ZLFile.h>
#include <ZLResource.h>
#include <ZLZDecompressor.h>

#include "EReaderStream.h"
#include "DocDecompressor.h"


EReaderStream::EReaderStream(ZLFile &file) : PalmDocLikeStream(file) {
	myDestination = TEXT;
}

EReaderStream::~EReaderStream() {
	close();
}

bool EReaderStream::switchStreamDestination(StreamDestination destination, const std::string& id) {
	bool result = true;
	switch(destination) {
		case TEXT:
			myDestination = TEXT; 
			myRecordIndex = 1;
			break;
		case FOOTNOTE:
			std::map<std::string, unsigned short>::const_iterator footnoteIt = myFootnotes.find(id);
			if (footnoteIt != myFootnotes.end()) {
				myDestination = FOOTNOTE; 
				myRecordIndex = footnoteIt->second;
			} else {
				result = false;
			}
			break;
	}
	return result;
}

bool EReaderStream::fillBuffer() {
	if (myDestination == TEXT) {
	 	return PalmDocLikeStream::fillBuffer();
	} else {
		while (myBufferOffset == myBufferLength) {
			if (!processRecord()) {
				return false;
			}
		}
		return true;
	}
}

bool EReaderStream::processRecord() {
	const size_t currentOffset = recordOffset(myRecordIndex);
	if (currentOffset < myBase->offset()) {
		return false;
	}
	myBase->seek(currentOffset, true);
	const size_t nextOffset = recordOffset(myRecordIndex + 1);
	if (nextOffset < currentOffset) {
		return false;
	}

	unsigned short myCompressedSize = nextOffset - currentOffset;
 
	switch (myCompressionVersion) {
		case 10:			// Inflate compression
			myBase->seek(2, false);
			myBufferLength = ZLZDecompressor(myCompressedSize - 2).decompress(*myBase, myBuffer, myMaxRecordSize); 
			break;
		case 2:				// PalmDoc compression
			myBufferLength = DocDecompressor().decompress(*myBase, myBuffer, myCompressedSize, myMaxRecordSize);
			break;
	}
	clearBuffer('\0');
	myBufferOffset = 0;
	return true;
}

bool EReaderStream::processZeroRecord() {
	// Use it with offset presetting to zero record offset value
	PdbUtil::readUnsignedShort(*myBase, myCompressionVersion); 			// myBase offset: ^ + 2
	if (myCompressionVersion > 255) {
		myErrorCode = ERROR_ENCRYPTION;
		return false;
	} else {
		switch (myCompressionVersion) {
			case 2:
			case 10:
				break;
			default:
				myErrorCode = ERROR_COMPRESSION;
				return false;
		}
	}
	myBase->seek(10, false);											// myBase offset: ^ + 12
	PdbUtil::readUnsignedShort(*myBase, myNonTextOffset); 				// myBase offset: ^ + 14
	PdbUtil::readUnsignedShort(*myBase, myNonTextOffsetReserved); 		// myBase offset: ^ + 16
	myBase->seek(12, false);											// myBase offset: ^ + 28
	PdbUtil::readUnsignedShort(*myBase, myFootnoteRecords); 			// myBase offset: ^ + 30
	PdbUtil::readUnsignedShort(*myBase, mySidebarRecords); 				// myBase offset: ^ + 32
	PdbUtil::readUnsignedShort(*myBase, myBookmarksOffset); 			// myBase offset: ^ + 34
	myBase->seek(2, false);												// myBase offset: ^ + 36
	PdbUtil::readUnsignedShort(*myBase, myNonTextOffsetExtraReserved); 	// myBase offset: ^ + 38
	myBase->seek(2, false);												// myBase offset: ^ + 40
	PdbUtil::readUnsignedShort(*myBase, myImagedataOffset); 			// myBase offset: ^ + 42
	PdbUtil::readUnsignedShort(*myBase, myImagedataOffsetReserved); 	// myBase offset: ^ + 44
	PdbUtil::readUnsignedShort(*myBase, myMetadataOffset); 				// myBase offset: ^ + 46
	PdbUtil::readUnsignedShort(*myBase, myMetadataOffsetReserved); 		// myBase offset: ^ + 48
	PdbUtil::readUnsignedShort(*myBase, myFootnoteOffset); 				// myBase offset: ^ + 50
	PdbUtil::readUnsignedShort(*myBase, mySidebarOffset); 				// myBase offset: ^ + 52
	PdbUtil::readUnsignedShort(*myBase, myLastdataOffset); 				// myBase offset: ^ + 54

	unsigned short endSectionIndex = header().Offsets.size();
	myMaxRecordIndex = std::min((unsigned short) (myNonTextOffset - 1), (unsigned short) (endSectionIndex - 1));

	myMaxRecordSize = 65535;	// Maximum size of addressable space in PalmOS 
								// not more than 8192 bytes happens in the tested examples
 
	if (myFootnoteRecords) {
		bool isSuccess = processFootnoteIdsRecord(); 
		if (!isSuccess) {
			//TODO take in account returned bool value
			//false if wrong footnotes amount anounced in zero record
			//or corrupted or wrong footnote ids record
		} 
	}

	if (myImagedataOffset != myMetadataOffset) {
		bool isSuccess = processImageHeaders(); 
		if (!isSuccess) {
			//TODO take in account returned bool value
			//false if one of image record is corrupted
		} 
	}

	myBase->seek(header().Offsets[1], true);

	/*
 	std::cerr << "EReaderStream::processZeroRecord():\n";
	std::cerr << "PDB header indentificator				: " << header().Id << "\n";
	std::cerr << "PDB file system: sizeof opened		: " << myBaseSize << "\n";
	std::cerr << "PDB header/record[0] max index		: " << myMaxRecordIndex << "\n";
	std::cerr << "PDB record[0][0..2] compression	 	: " << myCompressionVersion << "\n";
	std::cerr << "EReader record[0] myNonTextOffset 	: " << myNonTextOffset << std::endl;
	std::cerr << "EReader record[0] myNonTextOffset2 	: " << myNonTextOffsetReserved << std::endl;
	std::cerr << "EReader record[0] myFootnoteRecords 	: " << myFootnoteRecords << std::endl;
	std::cerr << "EReader record[0] mySidebarRecords 	: " << mySidebarRecords << std::endl;
	std::cerr << "EReader record[0] myBookmarksOffset 	: " << myBookmarksOffset << std::endl;
	std::cerr << "EReader record[0] myNonTextOffset3 	: " << myNonTextOffsetExtraReserved << std::endl;
	std::cerr << "EReader record[0] myImagedataOffset 	: " << myImagedataOffset << std::endl;
	std::cerr << "EReader record[0] myImagedataOffset2 	: " << myImagedataOffsetReserved << std::endl;
	std::cerr << "EReader record[0] myMetadataOffset 	: " << myMetadataOffset << std::endl;
	std::cerr << "EReader record[0] myMetadataOffset2 	: " << myMetadataOffsetReserved << std::endl;
	std::cerr << "EReader record[0] myFootnoteOffset 	: " << myFootnoteOffset << std::endl;
	std::cerr << "EReader record[0] mySidebarOffset 	: " << mySidebarOffset << std::endl;
	std::cerr << "EReader record[0] myLastdataOffset 	: " << myLastdataOffset << std::endl;
	std::cerr << "PDB header lastSectionIndex			: " << endSectionIndex - 1 << "\n";
	*/
	return true;
}

void EReaderStream::clearBuffer(unsigned char symbol) {
	myBufferLength = std::remove(myBuffer, myBuffer + myBufferLength, symbol) - myBuffer;
}

bool EReaderStream::processFootnoteIdsRecord() {
	char* footnoteIdBuffer = new char[myMaxRecordSize];
	myBase->seek(header().Offsets[myFootnoteOffset], true);
	const size_t currentOffset = recordOffset(myFootnoteOffset);
	const size_t nextOffset = recordOffset(myFootnoteOffset + 1);
	const size_t length = nextOffset - currentOffset;
	myBase->read(footnoteIdBuffer, length);
	std::string footnoteIdStr(footnoteIdBuffer, length);
	unsigned short footnoteIndex = myFootnoteOffset + 1;
	while (!footnoteIdStr.empty() && (footnoteIndex < myLastdataOffset)) {
		std::string id = findFootnoteId(footnoteIdStr);
		if (!id.empty()) {
			myFootnotes[id] = footnoteIndex;
			++footnoteIndex;
		}
	}
	delete[] footnoteIdBuffer;
	return (myFootnoteRecords - 1 == (unsigned short)myFootnotes.size());
}

std::string EReaderStream::findFootnoteId(std::string &footnoteIdStr) const {
	std::string resultStr;
	if (!footnoteIdStr.empty()) {
		size_t counter = 0;
		for (; counter < footnoteIdStr.length(); ++counter) {
			if (isalnum(footnoteIdStr[counter])) {
				break;
			}
		}
		const size_t startIdIndex = counter;
		for (; counter < footnoteIdStr.length(); ++counter) {
			if (footnoteIdStr[counter] == '\0') {
				break;
			}
		}
		const size_t endIdIndex = counter;
		resultStr = footnoteIdStr.substr(startIdIndex, endIdIndex - startIdIndex);
		footnoteIdStr = footnoteIdStr.substr(endIdIndex);
	}
	return resultStr;
}

const std::map<std::string, unsigned short>& EReaderStream::footnotes() const {
	return myFootnotes;
}

bool EReaderStream::processImageHeaders() {
	unsigned short recordIndex = myImagedataOffset;
	bool result = true;
	myBase->seek(header().Offsets[recordIndex], true);
	while (recordIndex < myMetadataOffset && recordIndex < myLastdataOffset) {
		result = result && addImageInfo(recordIndex);
		++recordIndex;
	}
	return result;
}

bool EReaderStream::addImageInfo(const unsigned short recordIndex) {
	const size_t bufferLength = 128;
	char *buffer = new char[bufferLength]; //TODO may be it's needed here more bytes
	ImageInfo image;
	const size_t currentOffset = recordOffset(recordIndex);
	const size_t nextOffset = recordOffset(recordIndex + 1);

	myBase->read(buffer, bufferLength);
	std::string header(buffer, bufferLength);
	delete[] buffer;

	image.Offset = currentOffset + header.find("\x89PNG"); //TODO treat situation when there isn't PNG in first 128 bytes
	image.Size = nextOffset - image.Offset;
	const int endType = header.find(' ');
	image.Type = header.substr(0, endType);
	header = header.substr(endType + 1);
	const int endId = header.find('\0');
	const std::string id = header.substr(0, endId);
	myBase->seek(nextOffset - currentOffset - bufferLength, false);
	if (id.empty()) {
		return false;
	}
	myImages[id] = image;
	return true;
}


/*bool EReaderStream::hasExtraSections() const {
	return false;
	//return myMaxRecordIndex < header().Offsets.size() - 1;
}*/

EReaderStream::ImageInfo EReaderStream::imageLocation(const std::string& id) {
	if (myImagedataOffset != myMetadataOffset && myImages.empty()) {
		processImageHeaders();
	}
	const std::map<const std::string, ImageInfo>::const_iterator it = myImages.find(id);
	if (it != myImages.end()) {
		return it->second;
	} else {
		return ImageInfo();
	}
}

const std::map<std::string, EReaderStream::ImageInfo>& EReaderStream::images() const {
	return myImages;
}
