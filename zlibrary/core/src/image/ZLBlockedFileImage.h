/*
 * Copyright (C) 2009-2012 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLBLOCKEDFILEIMAGE_H__
#define __ZLBLOCKEDFILEIMAGE_H__

#include <string>
#include <vector>

#include <ZLFile.h>
#include <ZLImage.h>

class ZLBlockedFileImage : public ZLSingleImage {

public:
	struct Block {
		unsigned int offset;
		unsigned int size;

		Block(unsigned int off, unsigned int s);
	};
	typedef std::vector<Block> Blocks;

public:
	ZLBlockedFileImage(const ZLFile &file, const Blocks &blocks);
	const shared_ptr<std::string> stringData() const;

private:
	const ZLFile myFile;
	const Blocks myBlocks;
};

inline ZLBlockedFileImage::Block::Block(unsigned int off, unsigned int s) : offset(off), size(s) { }

#endif /* __ZLBLOCKEDFILEIMAGE_H__ */
