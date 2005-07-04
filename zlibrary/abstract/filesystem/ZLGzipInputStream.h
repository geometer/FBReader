/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __ZLGZIPINPUTSTREAM_H__
#define __ZLGZIPINPUTSTREAM_H__

#define NOZLIBDEFS
#include <zlib.h>

#include "ZLInputStream.h"

class ZLGzipInputStream : public ZLInputStream {

public:
	ZLGzipInputStream(ZLInputStream *stream, size_t size) FS_SECTION;
	~ZLGzipInputStream() FS_SECTION;
	bool open() FS_SECTION;
	size_t read(char *buffer, size_t maxSize) FS_SECTION;
	void close() FS_SECTION;

	void seek(size_t offset) FS_SECTION;
	size_t offset() const FS_SECTION;

private:
	ZLInputStream *myFileStream;
	size_t myFileSize;

	z_stream *myZStream; 
	char *myInBuffer;
	char *myOutBuffer;

	std::string myBuffer; 
	size_t myAvailableSize;
	size_t myOffset;
};

#endif /* __ZLGZIPINPUTSTREAM_H__ */
