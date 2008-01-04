/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <string.h>

#include "BitStream.h"

const int BitStream::BufferSize = sizeof(unsigned int) * 8;

unsigned int BitStream::get4BytesDirect() {
	if (myByteStream + 4 > myByteStreamEnd) {
		return 0;
	}
	unsigned int bytes = *myByteStream++ << 24;
	bytes += *myByteStream++ << 16;
	bytes += *myByteStream++ << 8;
	bytes += *myByteStream++;
	return bytes;
}

bool BitStream::getBytesDirect(unsigned char *buffer, unsigned int length) {
	if (myByteStream + 4 > myByteStreamEnd) {
		return false;
	}
	memcpy(buffer, myByteStream, length);
	myByteStream += length;
	return true;
}
