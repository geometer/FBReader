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

#include <abstract/ZLInputStream.h>

#include "DocDecompressor.h"

size_t DocDecompressor::decompress(ZLInputStream &stream, char *buffer, size_t compressedSize, size_t maxUncompressedSize) {
	unsigned char *sourceBuffer = new unsigned char[compressedSize];

	unsigned int sourceIndex = 0;
	unsigned int targetIndex = 0;

	if (stream.read((char*)sourceBuffer, compressedSize) == compressedSize) {
		while ((sourceIndex < compressedSize) && (targetIndex < maxUncompressedSize)) {
			unsigned int token = sourceBuffer[sourceIndex++];
			if ((0 < token) && (token < 9)) {
				if ((sourceIndex + token > compressedSize) || (targetIndex + token > maxUncompressedSize)) {
					break;
				}
				memcpy(buffer + targetIndex, sourceBuffer + sourceIndex, token);
				sourceIndex += token;
				targetIndex += token;
			} else if (token < 0x80) {
				buffer[targetIndex++] = token;
			} else if (0xc0 <= token) {
				if (targetIndex + 2 > maxUncompressedSize) {
					break;
				}
				buffer[targetIndex++] = ' ';
				buffer[targetIndex++] = token ^ 0x80;
			} else {
				if (sourceIndex + 1 > compressedSize) {
					break;
				}
				token *= 256;
				token += sourceBuffer[sourceIndex++];
				int m = (token & 0x3fff) / 8;
				int n = (token & 7) + 3;
				if (targetIndex + n > maxUncompressedSize) {
					break;
				}
				memcpy(buffer + targetIndex, buffer + targetIndex - m, n);
				targetIndex += n;
			}
		}
	}

	delete[] sourceBuffer;
	return targetIndex;
}
