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

#include <string.h>
#include <stdio.h>

static char OLE_SIGN[] = {0xD0,0xCF,0x11,0xE0,0xA1,0xB1,0x1A,0xE1,0};

static const size_t BBD_BLOCK_SIZE = 512;
//static const int SBD_BLOCK_SIZE = 64; //not used
static const long PROP_BLOCK_SIZE = 128;

static const int OFFSET_SECTOR_SIZE = 0x1e;
static const int OFFSET_SHORT_SECTOR_SIZE = 0x20;

static const std::string ROOT_ENTRY = "Root Entry";

OleStorage::OleStorage()  {
	clear();
}

bool OleStorage::init(shared_ptr<ZLInputStream> stream, size_t streamSize)  {
	clear();

	myInputStream = stream;
	myStreamSize = streamSize;
	myInputStream->seek(0, true);

	char oleBuf[BBD_BLOCK_SIZE];
	size_t ret = myInputStream->read(oleBuf,BBD_BLOCK_SIZE);
	if (ret != BBD_BLOCK_SIZE) {
		clear();
		return false;
	}
	if (strncmp(oleBuf, OLE_SIGN, 8) != 0) {
		clear();
		return false;
	}
	mySectorSize = 1 << OleUtil::getUInt16(oleBuf, OFFSET_SECTOR_SIZE);
	myShortSectorSize = 1 << OleUtil::getUInt16(oleBuf, OFFSET_SHORT_SECTOR_SIZE);

	if (readBBD(oleBuf) && readSBD(oleBuf) && readProperties(oleBuf) && readAllEntries()) {
		return true;
	}
	clear();
	return false;
}

bool OleStorage::readMSAT(char* oleBuf, std::string& msat) {
	static const long int MSAT_NUMBER = 4;
	static const int MSAT_ORIG_SIZE = 436;
	static const int OFFSET_MSAT_BLOCK = 0x44;
	static const int OFFSET_MSAT_SIZE = 0x48;
	static const int OFFSET_MSAT_IN_HEADER = 0x4c;

	long int msatBlock = OleUtil::getInt32(oleBuf, OFFSET_MSAT_BLOCK);
	long int msatSize = OleUtil::getInt32(oleBuf, OFFSET_MSAT_SIZE);

	msat = std::string(oleBuf + OFFSET_MSAT_IN_HEADER, MSAT_ORIG_SIZE);

	int i=0;
	while(msatBlock >= 0 && i < msatSize) {
		ZLLogger::Instance().println("OleStorage", "Read additional data for MSAT");
		char buffer[mySectorSize];
		myInputStream->seek(BBD_BLOCK_SIZE + msatBlock * mySectorSize, true);
		size_t res = myInputStream->read(buffer, mySectorSize);
		if(res != mySectorSize) {
			ZLLogger::Instance().println("OleStorage", "Error read MSAT!");
			return false;
		}

		if (msat.size() >= MSAT_ORIG_SIZE + MSAT_NUMBER) {
			msat.resize(msat.size() - MSAT_NUMBER);
		}
		msat += std::string(buffer, mySectorSize);
		++i;
		msatBlock = OleUtil::getInt32(msat.c_str(), MSAT_ORIG_SIZE + (mySectorSize - 4) * i);
	}
	return true;
}

bool OleStorage::readBBD(char* oleBuf) {
	static const int OFFSET_BBD_NUM_BLOCKS = 0x2c;

	std::string msat;
	readMSAT(oleBuf, msat);

	char buffer[mySectorSize];
	unsigned long int bbdNumberBlocks = OleUtil::getUInt32(oleBuf, OFFSET_BBD_NUM_BLOCKS);

	for(int i = 0; i < bbdNumberBlocks; ++i) {
		long int bbdSector = OleUtil::getInt32(msat.c_str(), 4 * i);
		if (bbdSector >= myStreamSize/mySectorSize || bbdSector < 0) {
			ZLLogger::Instance().println("OleStorage", "Bad BBD entry!");
			return false;
		}
		myInputStream->seek(BBD_BLOCK_SIZE + bbdSector * mySectorSize, true);
		if (myInputStream->read(buffer, mySectorSize) != mySectorSize ) {
			ZLLogger::Instance().println("OleStorage", "Can't read BBD!");
			return false;
		}
		myBBD += std::string(buffer, mySectorSize);
	}
	return true;
}

bool OleStorage::readSBD(char* oleBuf) {
	static const int OFFSET_SBD_START = 0x3c;
	long int sbdStart, sbdCur;
	sbdCur = sbdStart = OleUtil::getInt32(oleBuf, OFFSET_SBD_START);
	if (sbdStart <= 0) {
		return true;
	}

	char buffer[mySectorSize];
	do {
		myInputStream->seek(BBD_BLOCK_SIZE + sbdCur * mySectorSize, true);
		myInputStream->read(buffer, mySectorSize);
		mySBD += std::string(buffer, mySectorSize);
		sbdCur = OleUtil::getInt32(myBBD.c_str(), sbdCur * 4);
	} while (sbdCur >=0 && sbdCur < myStreamSize / mySectorSize);

	//mySBDNumber = (mySBDLength * mySectorSize) / myShortSectorSize;
	return true;
}

bool OleStorage::readProperties(char* oleBuf) {
	static const int OFFSET_PROP_START = 0x30;
	long int propStart, propCur;
	propCur = propStart = OleUtil::getInt32(oleBuf, OFFSET_PROP_START);
	if (propStart < 0) {
		return false;
	}

	char buffer[mySectorSize];
	do {
		myInputStream->seek(BBD_BLOCK_SIZE + propCur * mySectorSize, true);
		myInputStream->read(buffer, mySectorSize);
		myProperties += std::string(buffer, mySectorSize);
		propCur = OleUtil::getInt32(myBBD.c_str(), propCur * 4);
	} while (propCur >= 0 && propCur < myStreamSize / mySectorSize);
	//myPropNumber = (myPropLength * mySectorSize) / PROP_BLOCK_SIZE;
	return true;
}

bool OleStorage::readAllEntries() {
	long propCount = myProperties.size() / PROP_BLOCK_SIZE;
	for (long i = 0; i < propCount; ++i) {
		OleEntry entry;
		bool result = readOleEntry(i, entry);
		if (!result) {
			break;
		}
		if (entry.type == OleEntry::ROOT_DIR) {
			myRootEntry = &entry;
		}
		myEntries.push_back(entry);
	}
	if (!myRootEntry) {
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

	char oleBuf[PROP_BLOCK_SIZE];
	for (long i = 0; i < PROP_BLOCK_SIZE; ++i) {
		oleBuf[i] = myProperties.at(i + propNumber * PROP_BLOCK_SIZE);
	}
	//unsigned char* oleBuf = (unsigned char*)myProperties.c_str() + propNumber * PROP_BLOCK_SIZE;

	char oleType = oleBuf[OFFSET_OLE_TYPE];
	if (oleType != 1 && oleType != 2 && oleType != 3 && oleType != 5) {
		ZLLogger::Instance().println("OleStorage", "entry -- not right ole type");
		return false;
	}

	e.type = (OleEntry::Type)oleType;
	e.startBlock = OleUtil::getInt32(oleBuf, OFFSET_START_BLOCK);

	int nameLength = OleUtil::getUInt16(oleBuf, OFFSET_NAME_LENGTH);
	e.name.clear();
	e.name.reserve(OLE_NAME_LENGTH + 1);
	for (int i = 0; i < nameLength; i+=2) {
		char c = oleBuf[i];
		if (c != 0) {
			e.name += c;
		}
	}

	e.length = OleUtil::getUInt32(oleBuf, OFFSET_ENTRY_LENGTH);

	// Read sector chain
	long int chainCur;
	chainCur = e.startBlock;
	e.isBigBlock = e.length >= 0x1000 || e.name == ROOT_ENTRY;

	if (e.startBlock >= 0 && e.length >= 0 &&
	   (e.startBlock <= myStreamSize / (e.isBigBlock ? mySectorSize : myShortSectorSize))) {

		//filling blocks with chains
		do {
			e.blocks.push_back(chainCur);
			if (e.isBigBlock) {
				chainCur = OleUtil::getInt32(myBBD.c_str(), chainCur * 4);
			} else if (!mySBD.empty()) {
				chainCur = OleUtil::getInt32(mySBD.c_str(), chainCur * 4);
			} else {
				chainCur = -1;
			}
		} while (chainCur > 0 &&
				 chainCur < e.isBigBlock ? (myBBD.size() / 4) : (mySBD.size() / 4) &&
				 e.blocks.size() <= e.length / (e.isBigBlock ? mySectorSize : myShortSectorSize));
	}
	e.length = std::min(e.length, (unsigned long int)((e.isBigBlock ? mySectorSize : myShortSectorSize) * e.blocks.size()));
	return true;
}

long int OleStorage::calcFileOffsetByBlockNumber(OleEntry& e, long int blockNumber) {
	long int res;
	if (e.isBigBlock) {
		res = BBD_BLOCK_SIZE + e.blocks.at(blockNumber) * mySectorSize;
	} else {
		long int sbdPerSector = mySectorSize / myShortSectorSize;
		long int sbdSecNum = e.blocks.at(blockNumber) / sbdPerSector;
		long int sbdSecMod = e.blocks.at(blockNumber) % sbdPerSector;
		res = BBD_BLOCK_SIZE + myRootEntry->blocks.at(sbdSecNum) * mySectorSize + sbdSecMod * myShortSectorSize;
	}
	return res;
}

void OleStorage::clear() {
	myInputStream = 0;
	mySectorSize = 0;
	myShortSectorSize = 0;
	myStreamSize = 0;
	myRootEntry = 0;

	myBBD.clear();
	mySBD.clear();
	myProperties.clear();
	myEntries.clear();
}

