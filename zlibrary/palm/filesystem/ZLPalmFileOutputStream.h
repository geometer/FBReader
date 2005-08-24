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

#ifndef __ZLPALMFILEOUTPUTSTREAM_H__
#define __ZLPALMFILEOUTPUTSTREAM_H__

#include <PalmOS.h>

#include "../../abstract/filesystem/ZLOutputStream.h"

class ZLPalmFileOutputStream : public ZLOutputStream {

public:
	ZLPalmFileOutputStream(const std::string &name) FS_SECTION;
	~ZLPalmFileOutputStream() FS_SECTION;
	bool open() FS_SECTION;
	void write(const std::string &str) FS_SECTION;
	void close() FS_SECTION;

private:
	ZLPalmFileOutputStream(const ZLPalmFileOutputStream&);
	const ZLPalmFileOutputStream operator = (const ZLPalmFileOutputStream&);

private:
	std::string myName;
	UInt32 myFileRef;
};

#endif /* __ZLPALMFILEOUTPUTSTREAM_H__ */
