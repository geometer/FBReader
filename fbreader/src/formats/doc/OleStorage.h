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

#include <ZLInputStream.h>

struct OleEntry {
	enum Type {
		DIR = 1,
		STREAM = 2,
		ROOT_DIR = 5,
		LOCK_BYTES =3
	};

	typedef std::vector<long int> BLOCKS;

	std::string name;
	long int startBlock;
	unsigned long int length;
	Type type;
	BLOCKS blocks;
	bool isBigBlock;
};

class OleStorage {

public:
	OleStorage();
	bool init(shared_ptr<ZLInputStream>, size_t streamSize);
	void clear();
	const std::vector<OleEntry>& getEntries() const;

	long int getSectorSize();
	long int getShortSectorSize();

public: //TODO make private
	long int calculateBlockOffset(OleEntry& e, long int blk);

private:
	bool readMSAT(char* oleBuf, std::string& MSAT);
	bool readBBD(char* oleBuf);
	bool readSBD(char* oleBuf);
	bool readProperties(char* oleBuf);

	bool readAllEntries();
	bool readOleEntry(long propNumber, OleEntry& entry);

private:

	shared_ptr<ZLInputStream> myInputStream;
	long int mySectorSize, myShortSectorSize;

	size_t myStreamSize;
	std::string myBBD; //Big Block Depot
	std::string mySBD; //Small Block Depot
	std::string myProperties;
	std::vector<OleEntry> myEntries;
	OleEntry *myRootEntry;

};

inline const std::vector<OleEntry>& OleStorage::getEntries() const { return myEntries; }
inline long int OleStorage::getSectorSize() { return mySectorSize; }
inline long int OleStorage::getShortSectorSize() { return myShortSectorSize; }

#endif /* __OLESTORAGE_H__ */
