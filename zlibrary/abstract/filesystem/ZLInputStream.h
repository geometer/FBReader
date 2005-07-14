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

#ifndef __ZLINPUTSTREAM_H__
#define __ZLINPUTSTREAM_H__

#include <string>

class ZLInputStream {

protected:
	ZLInputStream() FS_SECTION;

public:
	virtual ~ZLInputStream() FS_SECTION;
	virtual bool open() FS_SECTION = 0;
	virtual size_t read(char *buffer, size_t maxSize) FS_SECTION = 0;
	virtual void close() FS_SECTION = 0;

	virtual void seek(size_t offset) FS_SECTION = 0;
	virtual size_t offset() const FS_SECTION = 0;
	virtual size_t sizeOfOpened() FS_SECTION = 0;

private:
	// disable copying
	ZLInputStream(const ZLInputStream&) FS_SECTION;
	const ZLInputStream &operator = (const ZLInputStream&) FS_SECTION;
};

inline ZLInputStream::ZLInputStream() {}
inline ZLInputStream::~ZLInputStream() {}

#endif /* __ZLINPUTSTREAM_H__ */
