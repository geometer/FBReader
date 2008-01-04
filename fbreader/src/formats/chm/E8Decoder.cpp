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

#include "LZXDecompressor.h"

void LZXDecompressor::E8Decoder::reset(unsigned int fileSize) {
	myFileSize = fileSize;
	myFramesCounter = 0;
	myPosition = 0;
}

void LZXDecompressor::E8Decoder::decode(unsigned char *buffer, const size_t size) {
	if (myFramesCounter >= 32768) {
		return;
	}
	++myFramesCounter;
	if (myFileSize == 0) {
		return;
	}

	myPosition += size;

	if (size <= 10) {
		return;
	}
	
	const unsigned char *end = buffer + size - 10;

	for (unsigned char *ptr = buffer; ptr < end; ) {
		if (*ptr == 0xE8) {
			int absoluteOffset =
				ptr[1] + (ptr[2] << 8) + (ptr[3] << 16) + (ptr[4] << 24);
			int relativeOffset =
				(absoluteOffset >= 0) ?
					absoluteOffset - (ptr - buffer) : absoluteOffset + myFileSize;
			ptr[1] = (unsigned char)relativeOffset;
			ptr[2] = (unsigned char)(relativeOffset >> 8);
			ptr[3] = (unsigned char)(relativeOffset >> 16);
			ptr[4] = (unsigned char)(relativeOffset >> 24);
			ptr += 5;
		} else {
			++ptr;
		}
	}
}
