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

#ifndef __ZLUNICODEUTIL_H__
#define __ZLUNICODEUTIL_H__

#include <string>
#include <vector>

class ZLUnicodeUtil {

private:
	ZLUnicodeUtil() UTIL_SECTION;

public:
	typedef unsigned short Ucs2Char;
	typedef std::vector<Ucs2Char> Ucs2String;
	
	static int utf8Length(const char *str, int len) UTIL_SECTION;
	static int utf8Length(const std::string &str) UTIL_SECTION;
	static int length(const char *str, int utf8Length) UTIL_SECTION;
	static int length(const std::string &str, int utf8Length) UTIL_SECTION;
	static void utf8ToUcs2(Ucs2String &to, const std::string &from, int toLength = -1) UTIL_SECTION;
	static void ucs2ToUtf8(std::string &to, const Ucs2String &from, int toLength = -1) UTIL_SECTION;
	static bool isLetter(Ucs2Char ch) UTIL_SECTION;

	static Ucs2Char toLower(Ucs2Char ch) UTIL_SECTION;
	static void toLower(Ucs2String &str) UTIL_SECTION;
	static std::string toLower(const std::string &utf8String) UTIL_SECTION;

	static Ucs2Char toUpper(Ucs2Char ch) UTIL_SECTION;
	static void toUpper(Ucs2String &str) UTIL_SECTION;
	static std::string toUpper(const std::string &utf8String) UTIL_SECTION;
};

#endif /* __ZLUNICODEUTIL_H__ */
