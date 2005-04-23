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

#ifndef __ZLSTRINGINPUTSTREAM_H__
#define __ZLSTRINGINPUTSTREAM_H__

#include "ZLInputStream.h"

class ZLStringInputStream : public ZLInputStream {

public:
	ZLStringInputStream(const std::string &str) : myString(str) {}
	~ZLStringInputStream() {}
	bool open() { myPosition = 0; return true; }
	int read(char *buffer, int maxSize);
	int read(unsigned short *p);
	int read(unsigned long *p);
	void close() {}

	void seek(int offset) { myPosition += offset; }
	int offset() const { return myPosition; }

private:
	const std::string &myString;
	int myPosition;
};

#endif /* __ZSTRINGLINPUTSTREAM_H__ */
