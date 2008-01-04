/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#ifndef __PDBSTREAM_H__
#define __PDBSTREAM_H__

#include <ZLInputStream.h>

#include "PdbReader.h"

class ZLFile;

class PdbStream : public ZLInputStream {

public:
	PdbStream(ZLFile &file);
	virtual ~PdbStream();
	size_t read(char *buffer, size_t maxSize);
	virtual bool open();
	virtual void close();

	void seek(int offset, bool absoluteOffset);
	size_t offset() const;
	size_t sizeOfOpened();

protected:
	virtual bool fillBuffer() = 0;

protected:
	shared_ptr<ZLInputStream> myBase;
	size_t myOffset;
	PdbHeader myHeader;
	char *myBuffer;

	unsigned short myBufferLength;
	unsigned short myBufferOffset;
};

#endif /* __PDBSTREAM_H__ */
