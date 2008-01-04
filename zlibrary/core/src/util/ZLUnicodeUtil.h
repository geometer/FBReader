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

#ifndef __ZLUNICODEUTIL_H__
#define __ZLUNICODEUTIL_H__

#include <string>
#include <vector>

class ZLUnicodeUtil {

private:
	ZLUnicodeUtil();

public:
	typedef unsigned short Ucs2Char;
	typedef std::vector<Ucs2Char> Ucs2String;

	enum Breakable {
		NO_BREAKABLE,
		BREAKABLE_BEFORE,
		BREAKABLE_AFTER
	};
	
	static bool isUtf8String(const char *str, int len);
	static bool isUtf8String(const std::string &str);
	static int utf8Length(const char *str, int len);
	static int utf8Length(const std::string &str);
	static int length(const char *str, int utf8Length);
	static int length(const std::string &str, int utf8Length);
	static void utf8ToUcs2(Ucs2String &to, const char *from, int length, int toLength = -1);
	static void utf8ToUcs2(Ucs2String &to, const std::string &from, int toLength = -1);
	static int firstChar(Ucs2Char &ch, const char *utf8String);
	static void ucs2ToUtf8(std::string &to, const Ucs2String &from, int toLength = -1);
	static int ucs2ToUtf8(char *to, Ucs2Char ch);
	static bool isLetter(Ucs2Char ch);
	static bool isSpace(Ucs2Char ch);
	static bool isNBSpace(Ucs2Char ch);
	static Breakable isBreakable(Ucs2Char ch);

	static Ucs2Char toLower(Ucs2Char ch);
	static void toLower(Ucs2String &str);
	static std::string toLower(const std::string &utf8String);

	static Ucs2Char toUpper(Ucs2Char ch);
	static void toUpper(Ucs2String &str);
	static std::string toUpper(const std::string &utf8String);
};

inline bool ZLUnicodeUtil::isNBSpace(Ucs2Char ch) {
	return ch == 160;
}

#endif /* __ZLUNICODEUTIL_H__ */
