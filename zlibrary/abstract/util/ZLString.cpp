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

#include "ZLString.h"

ZLString::ZLString(const ZLString &s) {
	myLength = s.myLength;
	if (s.myData == 0) {
		myData = 0;
	} else {
		myData = new char[myLength];
		memcpy(myData, s.myData, myLength);
	}
}

const ZLString &ZLString::operator = (const ZLString &s) {
	if (myLength == s.myLength) {
		if (myData != 0) {
			memcpy(myData, s.myData, myLength);
		}
	} else {
		myLength = s.myLength;
		if (myData != 0) {
			delete[] myData;
		}
		if (s.myData == 0) {
			myData = 0;
		} else {
			myData = new char[myLength];
			memcpy(myData, s.myData, myLength);
		}
	}
	return *this;
}

void ZLString::reserve(size_t size) {
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

void ZLString::add(size_t offset, const std::string &s) {
	reserve(offset + s.length());
	memcpy(myData + offset, s.data(), s.length());
}

void ZLString::add(size_t offset, const ZLString &s) {
	reserve(offset + s.myLength);
	memcpy(myData + offset, s.myData, s.myLength);
}

void ZLString::append(const char *s, size_t len) {
	size_t offset = myLength;
	reserve(myLength + len);
	memcpy(myData + offset, s, len);
}

size_t ZLString::find(char c, size_t fromPos) const {
	if (fromPos > myLength) {
		return (size_t)-1;
	}

	char *end = myData + myLength;
	for (char *ptr = myData + fromPos; ptr != end; ++ptr) {
		if (*ptr == c) {
			return ptr - myData;
		}
	}
	return (size_t)-1;
}

void ZLString::operator += (const ZLStringBuffer &text) {
	size_t len = myLength;
	size_t offset = myLength;
	for (std::vector<ZLString>::const_iterator it = text.begin(); it != text.end(); it++) {
		len += it->length();
	}
	reserve(len);
	for (std::vector<ZLString>::const_iterator it = text.begin(); it != text.end(); it++) {
		add(offset, *it);
		offset += it->length();
	}
}
