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

ZLLowMemoryString::ZLLowMemoryString() {
	myLength = 0;
	myData = 0;
}

ZLLowMemoryString::~ZLLowMemoryString() {
	if (myData != 0) {
		delete[] myData;
	}
}

/*
ZLLowMemoryString::ZLLowMemoryString(const ZLLowMemoryString &s) {
	__myDataSize = STRING_TINY_DATA_SIZE;
	__myData = __myTinyData;
	operator = (s);
}

ZLLowMemoryString::ZLLowMemoryString(const char *s) {
	__myLength = 0;
	__myDataSize = STRING_TINY_DATA_SIZE;
	__myData = __myTinyData;

	size_t sLength = StrLen(s);
	if (sLength > 0) {
		reserve(sLength);
		MemMove(__myData, s, sLength);
		__myLength = sLength;
	}
}

const char *ZLLowMemoryString::c_str() const {
	reserve(__myLength + 1);
	__myData[__myLength] = '\0';
	return __myData;
}

const ZLLowMemoryString &ZLLowMemoryString::operator = (const ZLLowMemoryString &s) {
	__myLength = 0;
	if (s.__myLength > 0) {
		reserve(s.__myLength);
		__myLength = s.__myLength;
		MemMove(__myData, s.__myData, __myLength);
	}
	return *this;
}

const ZLLowMemoryString &ZLLowMemoryString::operator = (const char *s) {
	__myLength = 0;
	size_t len = StrLen(s);
	if (len > 0) {
		reserve(len);
		__myLength = len;
		MemMove(__myData, s, __myLength);
	}
	return *this;
}

bool ZLLowMemoryString::operator == (const ZLLowMemoryString &s) const {
	return (__myLength == s.__myLength) && (strncmp(__myData, s.__myData, __myLength) == 0);
}

bool ZLLowMemoryString::operator == (const char *s) const {
	return (__myLength == StrLen(s)) && (strncmp(__myData, s, __myLength - 1) == 0);
}

bool ZLLowMemoryString::operator < (const ZLLowMemoryString &s) const {
	size_t len = std::min(__myLength, s.__myLength);
	int result = strncmp(__myData, s.__myData, len);
	if (result < 0) {
		return true;
	} else if (result == 0) {
		return __myLength < s.__myLength;
	}
	return false;
}

void ZLLowMemoryString::swap(ZLLowMemoryString &s) {
	if ((__myData != __myTinyData) && (s.__myData != s.__myTinyData)) {
		char *p = __myData;
		__myData = s.__myData;
		s.__myData = p;
	} else if (__myData != __myTinyData) {
		s.__myData = __myData;
		__myData = __myTinyData;
		if (s.__myLength > 0) {
			MemMove(__myTinyData, s.__myTinyData, s.__myLength);
		}
	} else if (s.__myData != s.__myTinyData) {
		__myData = s.__myData;
		s.__myData = s.__myTinyData;
		if (__myLength > 0) {
			MemMove(s.__myTinyData, __myTinyData, __myLength);
		}
	} else if (__myLength > 0) {
		if (s.__myLength > 0) {
			// TODO: implement
		} else {
			MemMove(s.__myTinyData, __myTinyData, __myLength);
		}
	} else if (s.__myLength > 0) {
		MemMove(__myTinyData, s.__myTinyData, s.__myLength);
	}
	int l = __myDataSize; __myDataSize = s.__myDataSize; s.__myDataSize = l;
	l = __myLength; __myLength = s.__myLength; s.__myLength = l;
}

const ZLLowMemoryString &ZLLowMemoryString::operator += (const ZLLowMemoryString &s) {
	reserve(__myLength + s.__myLength);
	MemMove(__myData + __myLength, s.__myData, s.__myLength);
	__myLength += s.__myLength;
	return *this;
}

const ZLLowMemoryString &ZLLowMemoryString::operator += (char c) {
	reserve(__myLength + 1);
	__myData[__myLength] = c;
	__myLength++;
	return *this;
}

int ZLLowMemoryString::compare(const ZLLowMemoryString &s, int fromPos, int len) const {
	// TODO: check for short ZLLowMemoryString length < len (?)
	const char *ptr0 = __myData + fromPos;
	const char *ptr1 = s.__myData;
	const char *end = ptr0 + len;
	for (; ptr0 != end; ptr0++, ptr1++) {
		if (*ptr0 < *ptr1) {
			return -1;
		}
		if (*ptr0 > *ptr1) {
			return 1;
		}
	}
	
	return 0;
}
	
size_t ZLLowMemoryString::find(const ZLLowMemoryString &pattern, size_t fromPos) const {
	// TODO: implement
	return 0;
}

size_t ZLLowMemoryString::find(char c, size_t fromPos) const {
	if (fromPos > __myLength) {
		return (size_t)-1;
	}

	char *end = __myData + __myLength;
	for (char *ptr = __myData + fromPos; ptr != end; ++ptr) {
		if (*ptr == c) {
			return ptr - __myData;
		}
	}
	return (size_t)-1;
}

size_t ZLLowMemoryString::rfind(char c, size_t fromPos) const {
	if (fromPos > __myLength) {
		fromPos = __myLength;
	}
	char *end = __myData - 1;
	for (char *ptr = __myData + fromPos - 1; ptr != end; --ptr) {
		if (*ptr == c) {
			return ptr - __myData;
		}
	}
	return (size_t)-1;
}

ZLLowMemoryString ZLLowMemoryString::substr(size_t start, size_t len = (size_t)-1) const {
	if (start >= __myLength) {
		len = 0;
	} else if (len > __myLength - start) {
		len = __myLength - start;
	}
	ZLLowMemoryString result;
	result.reserve(len);
	MemMove(result.__myData, __myData + start, len);
	result.__myLength = len;
	return result;
}

void ZLLowMemoryString::erase(size_t start, size_t len) {
	if ((len == (size_t)-1) || (start + len >= __myLength)) {
		if (start < __myLength) {
			__myLength = start;
		}
	} else {
		__myLength -= len;
		MemMove(__myData + start, __myData + start + len, __myLength - start);
	}
}

const ZLLowMemoryString ZLLowMemoryString::operator + (const ZLLowMemoryString &s) const {
	ZLLowMemoryString sum;
	sum.reserve(__myLength + s.__myLength);
	MemMove(sum.__myData, __myData, __myLength);
	MemMove(sum.__myData + __myLength, s.__myData, s.__myLength);
	sum.__myLength = __myLength + s.__myLength;
	return sum;
}

const ZLLowMemoryString ZLLowMemoryString::operator + (char c) const {
	ZLLowMemoryString sum;
	sum.reserve(__myLength + 1);
	MemMove(sum.__myData, __myData, __myLength);
	sum.__myData[__myLength] = c;
	sum.__myLength = __myLength + 1;
	return sum;
}

const ZLLowMemoryString operator + (const char *s0, const ZLLowMemoryString &s1) {
	ZLLowMemoryString sum;
	int len0 = StrLen(s0);
	sum.reserve(len0 + s1.__myLength);
	MemMove(sum.__myData, s0, len0);
	MemMove(sum.__myData + len0, s1.__myData, s1.__myLength);
	sum.__myLength = len0 + s1.__myLength;
	return sum;
}
*/

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
