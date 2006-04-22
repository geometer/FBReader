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

#ifndef __ZLSTRINGINPUTSTREAM_H__
#define __ZLSTRINGINPUTSTREAM_H__

#include "ZLInputStream.h"

class ZLStringInputStream : public ZLInputStream {

public:
	ZLStringInputStream(const std::string &str) FS_SECTION;
	~ZLStringInputStream() FS_SECTION;
	bool open() FS_SECTION;
	size_t read(char *buffer, size_t maxSize) FS_SECTION;
	void close() FS_SECTION;

	void seek(int offset) FS_SECTION;
	size_t offset() const FS_SECTION;
	size_t sizeOfOpened() FS_SECTION;

private:
	const std::string &myString;
	size_t myPosition;
};

inline ZLStringInputStream::ZLStringInputStream(const std::string &str) : myString(str) {}
inline ZLStringInputStream::~ZLStringInputStream() {}
inline bool ZLStringInputStream::open() { myPosition = 0; return true; }
inline void ZLStringInputStream::close() {}
inline void ZLStringInputStream::seek(int offset) { myPosition += offset; }
inline size_t ZLStringInputStream::offset() const { return myPosition; }

#endif /* __ZSTRINGLINPUTSTREAM_H__ */
