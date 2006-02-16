/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __ZLBZIP2INPUTSTREAM_H__
#define __ZLBZIP2INPUTSTREAM_H__

#include "bzlib.h"

#include <abstract/shared_ptr.h>

#include "../ZLInputStream.h"

class ZLBzip2InputStream : public ZLInputStream {

private:
	ZLBzip2InputStream(shared_ptr<ZLInputStream> base) FS_SECTION;

public:
	~ZLBzip2InputStream() FS_SECTION;
	bool open() FS_SECTION;
	size_t read(char *buffer, size_t maxSize) FS_SECTION;
	void close() FS_SECTION;

	void seek(size_t offset) FS_SECTION;
	size_t offset() const FS_SECTION;
	size_t sizeOfOpened() FS_SECTION;

private:
	shared_ptr<ZLInputStream> myBaseStream;
	size_t myOffset;

	bz_stream myBzStream;
	char *myBaseBuffer;
	char *myTrashBuffer;
	size_t myBaseAvailableSize;

friend class ZLFile;
};

#endif /* __ZLBZIP2INPUTSTREAM_H__ */
