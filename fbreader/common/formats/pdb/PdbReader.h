/*
 * FBReader -- electronic book reader
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

#ifndef __PDBREADER_H__
#define __PDBREADER_H__

#include <vector>

#include <abstract/shared_ptr.h>
#include <abstract/ZLInputStream.h>

class BookModel;

class PdbUtil {

public:
	static void readUnsignedShort(shared_ptr<ZLInputStream> stream, unsigned short &N);
	static void readUnsignedLong(shared_ptr<ZLInputStream> stream, unsigned long &N);
};

struct PdbHeader {
	std::string DocName;
	unsigned short Flags;
	std::string Id;
	std::vector<unsigned long> Offsets;

	bool read(shared_ptr<ZLInputStream> stream) FORMATS_SECTION;
};

#endif /* __PDBREADER_H__ */
