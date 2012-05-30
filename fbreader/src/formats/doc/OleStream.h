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

#ifndef __OLESTREAM_H__
#define __OLESTREAM_H__

#include "OleStorage.h"

class OleStream {

public:
	OleStream(shared_ptr<OleStorage> storage, OleEntry oleEntry,shared_ptr<ZLInputStream> stream);

	bool open();
	bool eof();
	bool close();

	size_t read(char *ptr, size_t size, size_t nmemb);
	bool seek(long offset, bool absoluteOffset);
	long tell();

public:
	long getTextLength() const;

public: //TODO make private
	shared_ptr<OleStorage> myStorage; //for calling calcFileOffsetByBlockNumber function

private:
	OleEntry myOleEntry;
	shared_ptr<ZLInputStream> myInputStream;

	long myCurBlock;
	long myOleOffset;
	//long myFileOffset;

	long myTextLength;


};

inline long OleStream::getTextLength() const { return myTextLength; }

#endif /* __OLESTREAM_H__ */
