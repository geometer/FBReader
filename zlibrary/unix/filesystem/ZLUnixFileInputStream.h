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

#ifndef __ZLUNIXFILEINPUTSTREAM_H__
#define __ZLUNIXFILEINPUTSTREAM_H__

#include <stdio.h>

#include <abstract/ZLInputStream.h>

class ZLUnixFileInputStream : public ZLInputStream {

public:
	ZLUnixFileInputStream(const std::string &name);
	~ZLUnixFileInputStream();
	bool open();
	size_t read(char *buffer, size_t maxSize);
	void close();

	void seek(size_t offset) { fseek(myFile, offset, SEEK_CUR); }
	size_t offset() const { return ftell(myFile); }
	size_t sizeOfOpened();

private:
	std::string myName;
	FILE *myFile;
};

#endif /* __ZLUNIXFILEINPUTSTREAM_H__ */
