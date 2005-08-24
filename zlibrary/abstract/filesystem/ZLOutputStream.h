/*
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

#ifndef __ZLOUTPUTSTREAM_H__
#define __ZLOUTPUTSTREAM_H__

#include <string>

class ZLOutputStream {

protected:
	ZLOutputStream() FS_SECTION;

public:
	virtual ~ZLOutputStream() FS_SECTION;
	virtual bool open() FS_SECTION = 0;
	virtual void write(const std::string &str) FS_SECTION = 0;
	virtual void close() FS_SECTION = 0;

private:
	ZLOutputStream(const ZLOutputStream&) FS_SECTION;
	const ZLOutputStream &operator = (const ZLOutputStream&) FS_SECTION;
};

inline ZLOutputStream::ZLOutputStream() {}
inline ZLOutputStream::~ZLOutputStream() {}

#endif /* __ZLOUTPUTSTREAM_H__ */
