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

#include "OleStorage.h"
#include "OleUtil.h"

#include <cstring>

static char OLE_SIGN[] = {0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1, 0};

static const size_t BBD_BLOCK_SIZE = 512;
//static const int SBD_BLOCK_SIZE = 64; //not used
static const long PROP_BLOCK_SIZE = 128;

static const int OFFSET_SECTOR_SIZE = 0x1e;
static const int OFFSET_SHORT_SECTOR_SIZE = 0x20;

static const int OFFSET_MAJOR_VERSION = 0x1A;
static const int OFFSET_MINOR_VERSION = 0x18;

static const long ENDOFCHAIN = 0xFFFFFFFE;
static const long FREESECT = 0xFFFFFFFF;

static const std::string ROOT_ENTRY = "Root Entry";

OleStorage::OleStorage() {
	clear();
}

void OleStorage::clear() {
	myInputStream = 0;
	mySectorSize = 0;
	myShortSectorSize = 0;
	myStreamSize = 0;
	myRootEntryIndex = -1;

	myDIFAT.clear();
	myBBD.clear();
	mySBD.clear();
	myProperties.clear();
	myEntries.clear();
}



bool OleStorage::init(shared_ptr<ZLInputStream> stream, size_t streamSize) {
	clear();

	myInputStream = stream;
	myStreamSize = streamSize;
	myInputStream->seek(0, true);

	char oleBuf[BBD_BLOCK_SIZE];
	size_t ret = myInputStream->read(oleBuf, BBD_BLOCK_SIZE);
	if (ret != BBD_BLOCK_SIZE) {
		clear();
		return false;
	}
	if (strncmp(oleBuf, OLE_SIGN, 8) != 0) {
		clear();
		return false;
	}
	mySectorSize = 1 << OleUtil::getUShort(oleBuf, OFFSET_SECTOR_SIZE);
	myShortSectorSize = 1 << OleUtil::getUShort(oleBuf, OFFSET_SHORT_SECTOR_SIZE);

	//printf("Major version: %u, minor version: %u\n", OleUtil::getUShort(oleBuf, OFFSET_MAJOR_VERSION),
	//	  OleUtil::getUShort(oleBuf, OFFSET_MINOR_VERSION));
	//printf("mySectorSize: %u, myShortSectorSize = %u\n", mySectorSize, myShortSectorSize);

	if (readDIFAT(oleBuf) && readBBD(oleBuf) && readSBD(oleBuf) && readProperties(oleBuf) && readAllEntries()) {
		return true;
	}
	clear();
	return false;
}

bool OleStorage::readDIFAT(char* oleBuf) {
	static const int DIFAT_IN_HEADER_SIZE = 436;
	static const int OFFSET_FIRST_DIFAT_BLOCK = 0x44;
	static const int OFFSET_DIFAT_SECTOR_NUMBERS = 0x48;
	static const int OFFSET_DIFAT_IN_HEADER = 0x4c;

	long difatBlock = OleUtil::getLong(oleBuf, OFFSET_FIRST_DIFAT_BLOCK);
	long difatSectorNumbers = OleUtil::getLong(oleBuf, OFFSET_DIFAT_SECTOR_NUMBERS);

	for (int i = 0; i < DIFAT_IN_HEADER_SIZE; i += 4) {
		myDIFAT.push_back(OleUtil::getLong(oleBuf + OFFSET_DIFAT_IN_HEADER, i));
	}

	//for files > 6.78 mb we need read additional DIFAT fields
	for (int i = 0; difatBlock > 0 && i < difatSectorNumbers; ++i) {
		ZLLogger::Instance().println("OleStorage", "Read additional data for DIFAT");
		char buffer[mySectorSize];
		myInputStream->seek(BBD_BLOCK_SIZE + difatBlock * mySectorSize, true);
		if (myInputStream->read(buffer, mySectorSize) != mySectorSize) {
			ZLLogger::Instance().println("OleStorage", "Error read DIFAT!");
			return false;
		}
		for (unsigned int j = 0; j < (mySectorSize - 4); j += 4) {
			myDIFAT.push_back(OleUtil::getLong(buffer, j));
		}
		difatBlock = OleUtil::getLong(buffer, mySectorSize - 4); //next DIFAT block is pointed at the end of the sector
	}

	//removing unusable DIFAT links
	while (myDIFAT.back() == FREESECT) {
		myDIFAT.pop_back();
	}
	return true;
}

bool OleStorage::readBBD(char* oleBuf) {
	static const int OFFSET_BBD_NUM_BLOCKS = 0x2c;

	char buffer[mySectorSize];
	unsigned long bbdNumberBlocks = OleUtil::getULong(oleBuf, OFFSET_BBD_NUM_BLOCKS);

	//printf("bbdNumberBlocks: %lu\n", bbdNumberBlocks);

	for (unsigned long i = 0; i < bbdNumberBlocks; ++i) {
		long bbdSector = myDIFAT.at(i);
		if (bbdSector >= (long)(myStreamSize/mySectorSize) || bbdSector < 0) {
			ZLLogger::Instance().println("OleStorage", "Bad BBD entry!");
			return false;
		}
		myInputStream->seek(BBD_BLOCK_SIZE + bbdSector * mySectorSize, true);
		if (myInputStream->read(buffer, mySectorSize) != mySectorSize) {
			ZLLogger::Instance().println("OleStorage", "Can't read BBD!");
			return false;
		}
		for (unsigned int j = 0; j < mySectorSize; j += 4) {
			myBBD.push_back(OleUtil::getLong(buffer, j));
		}
	}

	//printf("Length of BBD: %d\n", myBBD.size());
	return true;
}

bool OleStorage::readSBD(char* oleBuf) {
	static const int OFFSET_SBD_START = 0x3c;
	static const int OFFSET_SBD_COUNT = 0x40;

	long sbdCur = OleUtil::getLong(oleBuf, OFFSET_SBD_START);
	long sbdCount = OleUtil::getLong(oleBuf, OFFSET_SBD_COUNT);

	if (sbdCur <= 0) {
		ZLLogger::Instance().println("OleStorage", "There's no SBD, don't read it");
		return true;
	}
	//printf("sbdCount = %ld\n", sbdCount);

	char buffer[mySectorSize];
	for (long i = 0; i < sbdCount; ++i) {
		if (i != 0) {
			sbdCur = myBBD.at(sbdCur);
		}
		if (sbdCur <= 0) {
			break;
		}
		//printf("sbdCur = %ld\n", sbdCur);
		myInputStream->seek(BBD_BLOCK_SIZE + sbdCur * mySectorSize, true);
		myInputStream->read(buffer, mySectorSize);
		for (unsigned int j = 0; j < mySectorSize; j += 4) {
			mySBD.push_back(OleUtil::getLong(buffer, j));
		}

	}
	return true;
}

bool OleStorage::readProperties(char* oleBuf) {
	static const int OFFSET_PROP_START = 0x30;

	long propCur = OleUtil::getLong(oleBuf, OFFSET_PROP_START);
	if (propCur < 0) {
		ZLLogger::Instance().println("OleStorage", "Wrong first directory sector location");
		return false;
	}

	char buffer[mySectorSize];
	do {
		myInputStream->seek(BBD_BLOCK_SIZE + propCur * mySectorSize, true);
		myInputStream->read(buffer, mySectorSize);
		for (unsigned int j = 0; j < mySectorSize; j += 128) {
			myProperties.push_back(std::string(buffer + j, 128));
		}
		if (propCur < 0 || (size_t)propCur >= myBBD.size()) {
			break;
		}
		propCur = myBBD.at(propCur);
	} while (propCur >= 0 && propCur < (long)(myStreamSize / mySectorSize));
	return true;
}

bool OleStorage::readAllEntries() {
	long propCount = myProperties.size();
	//printf("propCount = %u\n", myProperties.size());
	for (long i = 0; i < propCount; ++i) {
		OleEntry entry;
		bool result = readOleEntry(i, entry);
		if (!result) {
			break;
		}
		//printf("%ld entry name is %s\n", i, entry.name.c_str());
		if (entry.type == OleEntry::ROOT_DIR) {
			myRootEntryIndex = i;
		}
		myEntries.push_back(entry);
	}
	if (myRootEntryIndex < 0) {
		return false;
	}
	return true;
}

bool OleStorage::readOleEntry(long propNumber, OleEntry& e) {
	static const size_t OLE_NAME_LENGTH = 32;
	static const int OFFSET_OLE_TYPE = 0x42;
	static const int OFFSET_START_BLOCK = 0x74;
	static const int OFFSET_NAME_LENGTH = 0x40;
	static const int OFFSET_ENTRY_LENGTH = 0x78;

	std::string property = myProperties.at(propNumber);

	char oleType = property.at(OFFSET_OLE_TYPE);
	if (oleType != 1 && oleType != 2 && oleType != 3 && oleType != 5) {
		ZLLogger::Instance().println("OleStorage", "entry -- not right ole type");
		return false;
	}

	e.type = (OleEntry::Type)oleType;

	int nameLength = OleUtil::getUShort(property.c_str(), OFFSET_NAME_LENGTH);
	e.name.clear();
	e.name.reserve(OLE_NAME_LENGTH + 1);
	for (int i = 0; i < nameLength; i+=2) {
		char c = property.at(i);
		if (c != 0) {
			e.name += c;
		}
	}

	e.length = OleUtil::getULong(property.c_str(), OFFSET_ENTRY_LENGTH);
	e.isBigBlock = e.length >= 0x1000 || e.name == ROOT_ENTRY;

	// Read sector chain
	long chainCur = OleUtil::getLong(property.c_str(), OFFSET_START_BLOCK);
	if  (chainCur >= 0 &&
		(chainCur <= (long)(myStreamSize / (e.isBigBlock ? mySectorSize : myShortSectorSize)))) {

		//filling blocks with chains
		do {
			e.blocks.push_back((unsigned long)chainCur);
			if (e.isBigBlock && (size_t)chainCur < myBBD.size()) {
				chainCur = myBBD.at(chainCur);
			} else if (!mySBD.empty() && (size_t)chainCur < mySBD.size()) {
				chainCur = mySBD.at(chainCur);
			} else {
				chainCur = -1;
			}
		} while (chainCur > 0 &&
				chainCur < (e.isBigBlock ? myBBD.size() : mySBD.size()) &&
				e.blocks.size() <= e.length / (e.isBigBlock ? mySectorSize : myShortSectorSize));
	}
	e.length = std::min(e.length, (unsigned long)(e.isBigBlock ? mySectorSize : myShortSectorSize) * e.blocks.size());
	return true;
}

unsigned long OleStorage::calcFileOffsetByBlockNumber(OleEntry& e, unsigned int blockNumber) {
	unsigned long res;
	if (e.isBigBlock) {
		res = BBD_BLOCK_SIZE + e.blocks.at(blockNumber) * mySectorSize;
	} else {
		unsigned long sbdPerSector = mySectorSize / myShortSectorSize;
		unsigned long sbdSecNum = e.blocks.at(blockNumber) / sbdPerSector;
		unsigned long sbdSecMod = e.blocks.at(blockNumber) % sbdPerSector;
		res = BBD_BLOCK_SIZE + myEntries.at(myRootEntryIndex).blocks.at(sbdSecNum) * mySectorSize + sbdSecMod * myShortSectorSize;
	}
	return res;
}

bool OleStorage::getEntryByName(std::string name, OleEntry &returnEntry) const {
	for (size_t i = 0; i < myEntries.size(); ++i) {
		const OleEntry& entry = myEntries.at(i);
		if (entry.name == name) {
			returnEntry = entry;
			return true;
		}
	}
	return false;
}
