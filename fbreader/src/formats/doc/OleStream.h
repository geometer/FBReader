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

	size_t read(void *ptr, size_t size, size_t nmemb);
	int seek(long offset, bool absoluteOffset);
	long tell();

public:
	long getLength() const;

public: //TODO make private
	shared_ptr<OleStorage> myStorage; //for calling calculateBlockOffset function

private:
	OleEntry myOleEntry;
	shared_ptr<ZLInputStream> myInputStream;

	long int myCurBlock;
	long int myOleOffset;
	long int myFileOffset;

	long myLength; //TODO ole entry has a length yet?


};

inline long OleStream::getLength() const { return myLength; }

#endif /* __OLESTREAM_H__ */
