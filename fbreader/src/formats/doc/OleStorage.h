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

#ifndef __OLESTORAGE_H__
#define __OLESTORAGE_H__


#include <algorithm>
#include <vector>
#include <string>

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <errno.h>
#include <stdint.h>

#include <ZLInputStream.h>

struct OleEntry {
	enum Type {
		DIR = 1,
		STREAM = 2,
		ROOT_DIR = 5,
		LOCK_BYTES =3
	};

	typedef std::vector<unsigned long> Blocks;

	std::string name;
	unsigned long length;
	Type type;
	Blocks blocks;
	bool isBigBlock;
};

class OleStorage {

public:
	OleStorage();
	bool init(shared_ptr<ZLInputStream>, size_t streamSize);
	void clear();
	const std::vector<OleEntry>& getEntries() const;
	bool getEntryByName(std::string name, OleEntry& entry) const;

	unsigned int getSectorSize();
	unsigned int getShortSectorSize();

public: //TODO make private
	unsigned long calcFileOffsetByBlockNumber(OleEntry& e, unsigned int  blockNumber);

private:
	bool readDIFAT(char* oleBuf);
	bool readBBD(char* oleBuf);
	bool readSBD(char* oleBuf);
	bool readProperties(char* oleBuf);

	bool readAllEntries();
	bool readOleEntry(long propNumber, OleEntry& entry);

private:

	shared_ptr<ZLInputStream> myInputStream;
	unsigned int mySectorSize, myShortSectorSize;

	size_t myStreamSize;
	std::vector<long> myDIFAT; //double-indirect file allocation table
	std::vector<long> myBBD; //Big Block Depot
	std::vector<long> mySBD; //Small Block Depot
	std::vector<std::string> myProperties;
	std::vector<OleEntry> myEntries;
	long myRootEntryIndex;

};

inline const std::vector<OleEntry>& OleStorage::getEntries() const { return myEntries; }
inline unsigned int OleStorage::getSectorSize() { return mySectorSize; }
inline unsigned int OleStorage::getShortSectorSize() { return myShortSectorSize; }

#endif /* __OLESTORAGE_H__ */
