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

#ifndef __ZLLOWMEMORYSTRING_H__
#define __ZLLOWMEMORYSTRING_H__

#include <string>

class ZLLowMemoryString {

public:
	ZLLowMemoryString() UTIL_SECTION;
	~ZLLowMemoryString() UTIL_SECTION;
	//ZLLowMemoryString(const ZLLowMemoryString &s) UTIL_SECTION;
	//ZLLowMemoryString(const char *s) UTIL_SECTION;
	//const ZLLowMemoryString &operator = (const ZLLowMemoryString &s) UTIL_SECTION;
	//const ZLLowMemoryString &operator = (const char *s) UTIL_SECTION;
	
	void reserve(size_t size) UTIL_SECTION;
	void erase() UTIL_SECTION;
	//void erase(size_t start, size_t len = (size_t)-1) UTIL_SECTION;
	//const ZLLowMemoryString &operator += (const ZLLowMemoryString &s) UTIL_SECTION;
	//const ZLLowMemoryString &operator += (char c) UTIL_SECTION;
	//const ZLLowMemoryString operator + (const ZLLowMemoryString &s) const UTIL_SECTION;
	//const ZLLowMemoryString operator + (char c) const UTIL_SECTION;
	void add(size_t offset, const std::string &s) UTIL_SECTION;

	size_t length() const UTIL_SECTION;
	bool empty() const UTIL_SECTION;
	const char *data() const UTIL_SECTION;
	//const char *c_str() const UTIL_SECTION;
	char &operator [] (size_t index) const UTIL_SECTION;

	//ZLLowMemoryString substr(size_t start, size_t len = (size_t)-1) const UTIL_SECTION;
	//void append(const char *s, size_t len) UTIL_SECTION;
	//void swap(ZLLowMemoryString &s) UTIL_SECTION;

	//int compare(const ZLLowMemoryString &s, int fromPos, int len) const UTIL_SECTION;
	//bool operator == (const ZLLowMemoryString &s) const UTIL_SECTION;
	//bool operator != (const ZLLowMemoryString &s) const UTIL_SECTION;
	//bool operator == (const char *s) const UTIL_SECTION;
	//bool operator != (const char *s) const UTIL_SECTION;
	//bool operator < (const ZLLowMemoryString &s) const UTIL_SECTION;
	//
	//size_t find(const ZLLowMemoryString &pattern, size_t fromPos = 0) const UTIL_SECTION;
	//size_t rfind(const ZLLowMemoryString &pattern, size_t fromPos = (size_t)-1) const UTIL_SECTION;
	//size_t find(char c, size_t fromPos = 0) const UTIL_SECTION;
	//size_t rfind(char c, size_t fromPos = (size_t)-1) const UTIL_SECTION;

private:
	size_t myLength;
	char *myData;

//friend const ZLLowMemoryString operator + (const char *s0, const ZLLowMemoryString &s1) UTIL_SECTION;

private:
	ZLLowMemoryString(const ZLLowMemoryString &s) UTIL_SECTION;
	const ZLLowMemoryString &operator = (const ZLLowMemoryString &s) UTIL_SECTION;
};

//const ZLLowMemoryString operator + (const char *s0, const ZLLowMemoryString &s1) UTIL_SECTION;
//bool operator == (const char *s0, const ZLLowMemoryString &s1) UTIL_SECTION;
//bool operator != (const char *s0, const ZLLowMemoryString &s1) UTIL_SECTION;

inline size_t ZLLowMemoryString::length() const { return myLength; }
inline bool ZLLowMemoryString::empty() const { return myLength == 0; }
inline const char *ZLLowMemoryString::data() const { return myData; }
inline char &ZLLowMemoryString::operator [] (size_t index) const { return myData[index]; }
//inline bool ZLLowMemoryString::operator != (const ZLLowMemoryString &s) const { return !operator == (s); }
//inline bool ZLLowMemoryString::operator != (const char *s) const { return !operator == (s); }
//inline bool operator == (const char *s0, const ZLLowMemoryString &s1) { return s1 == s0; }
//inline bool operator != (const char *s0, const ZLLowMemoryString &s1) { return s1 != s0; }
inline void ZLLowMemoryString::erase() { if (myData != 0) { delete[] myData; myData = 0; } myLength = 0; }

#endif /* __ZLLOWMEMORYSTRING_H__ */
