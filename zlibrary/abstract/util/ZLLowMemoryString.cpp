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

#include <algorithm>

#include "ZLLowMemoryString.h"

void ZLLowMemoryString::reserve(size_t size) {
	if (size > myLength) {
		if (myData == 0) {
			myData = new char[size];
		} else {
			char *d = new char[size];
			memcpy(d, myData, myLength);
			delete[] myData;
			myData = d;
		}
		myLength = size;
	}
}

void ZLLowMemoryString::add(size_t offset, const std::string &s) {
	reserve(offset + s.length());
	memcpy(myData + offset, s.data(), s.length());
}

/*
void ZLLowMemoryString::append(const char *s, size_t len) {
	reserve(__myLength + len);
	MemMove(__myData + __myLength, s, len);
	__myLength += len;
}
*/
