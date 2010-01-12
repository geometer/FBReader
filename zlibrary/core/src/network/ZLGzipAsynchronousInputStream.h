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

#ifndef __ZLGZIPASYNCHRONOUSINPUTSTREAM_H__
#define __ZLGZIPASYNCHRONOUSINPUTSTREAM_H__

#include <zlib.h>
#include <shared_ptr.h>

#include "ZLAsynchronousInputStream.h"


class ZLGzipAsynchronousInputStream : public ZLAsynchronousInputStream  {

public:
	ZLGzipAsynchronousInputStream(const char *encoding = 0);
	~ZLGzipAsynchronousInputStream();

private:
	bool processInputInternal(Handler &handler);

private:
	bool skipHeader(const char *data, size_t len);

private:
	// GZip method data
	z_stream *myZStream;
	char *myOutBuffer;
	const size_t myOutBufferSize;
	bool myEndOfStream;

	// Skip header data	
	int myStreamState;
	size_t myOffset;
	unsigned char myFlag;
	unsigned char myByte;
};

#endif /* __ZLGZIPASYNCHRONOUSINPUTSTREAM_H__ */
