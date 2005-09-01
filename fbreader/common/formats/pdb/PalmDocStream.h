/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#ifndef __PALMDOCSTREAM_H__
#define __PALMDOCSTREAM_H__

#include <vector>

#include <abstract/ZLInputStream.h>

#include "PdbReader.h"

class ZLFile;

class PalmDocStream : public ZLInputStream {

public:
	PalmDocStream(ZLFile &file);
	~PalmDocStream();
	bool open();
	size_t read(char *buffer, size_t maxSize);
	void close();

	void seek(size_t offset);
	size_t offset() const;
	size_t sizeOfOpened();

private:
	bool fillBuffer();

private:
	shared_ptr<ZLInputStream> myBase;
	size_t myOffset;
	bool myIsCompressed;
	std::vector<size_t> myRecordSizes;
	PdbHeader myHeader;
	char *myBuffer;

	size_t myMaxRecordIndex;
	unsigned short myMaxRecordSize;
	size_t myRecordIndex;
	unsigned short myBufferLength;
	unsigned short myBufferOffset;
};

#endif /* __PALMDOCSTREAM_H__ */
