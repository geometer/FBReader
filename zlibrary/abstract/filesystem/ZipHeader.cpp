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

#include "ZipHeader.h"
#include "ZLInputStream.h"

bool ZipHeader::readFrom(ZLInputStream &stream) {
	int startOffset = stream.offset();
	stream.read((char*)&Signature, 4);
	stream.read((char*)&Version, 2);
	stream.read((char*)&Flags, 2);
	stream.read((char*)&CompressionMethod, 2);
	stream.read((char*)&ModificationTime, 2);
	stream.read((char*)&ModificationDate, 2);
	stream.read((char*)&CRC32, 4);
	stream.read((char*)&CompressedSize, 4);
	stream.read((char*)&UncompressedSize, 4);
	stream.read((char*)&NameLength, 2);
	stream.read((char*)&ExtraLength, 2);
	return (Signature == 0x04034B50) && (stream.offset() == startOffset + 30) && (NameLength != 0);
}
