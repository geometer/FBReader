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

#ifndef __ZLSTRING_H__
#define __ZLSTRING_H__

#include <string>
#include <vector>

class ZLString;
typedef std::vector<ZLString> ZLStringBuffer;

class ZLString {

public:
	ZLString() UTIL_SECTION;
	~ZLString() UTIL_SECTION;
	ZLString(const ZLString &s) UTIL_SECTION;
	const ZLString &operator = (const ZLString &s) UTIL_SECTION;
	
	void reserve(size_t size) UTIL_SECTION;

	void erase() UTIL_SECTION;
	void add(size_t offset, const std::string &s) UTIL_SECTION;
	void add(size_t offset, const ZLString &s) UTIL_SECTION;
	void operator += (const std::string &s) UTIL_SECTION;
	void operator += (const ZLString &s) UTIL_SECTION;
	void operator += (const ZLStringBuffer &text) UTIL_SECTION;
	void append(const char *s, size_t len) UTIL_SECTION;
	void swap(ZLString &s) UTIL_SECTION;

	size_t length() const UTIL_SECTION;
	bool empty() const UTIL_SECTION;
	const char *data() const UTIL_SECTION;
	char &operator [] (size_t index) const UTIL_SECTION;
	size_t find(char c, size_t fromPos = 0) const UTIL_SECTION;

private:
	size_t myLength;
	char *myData;
};

inline ZLString::ZLString() : myLength(0), myData(0) {}
inline ZLString::~ZLString() { if (myData != 0) delete[] myData; }

inline size_t ZLString::length() const { return myLength; }
inline bool ZLString::empty() const { return myLength == 0; }
inline const char *ZLString::data() const { return myData; }
inline char &ZLString::operator [] (size_t index) const { return myData[index]; }

inline void ZLString::erase() { if (myData != 0) { delete[] myData; myData = 0; } myLength = 0; }
inline void ZLString::operator += (const std::string &s) { add(myLength, s); }
inline void ZLString::operator += (const ZLString &s) { add(myLength, s); }
inline void ZLString::swap(ZLString &s) {
	size_t tmpLength = myLength; myLength = s.myLength; s.myLength = tmpLength;
	char *tmpData = myData; myData = s.myData; s.myData = tmpData;
}

#endif /* __ZLSTRING_H__ */
