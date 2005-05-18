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

#include "ZLUnicodeUtil.h"

int ZLUnicodeUtil::utf8Length(const char *str, int len) {
	const char *last = str + len;
	int counter = 0;
	while (str < last) {
		if ((*str & 0x80) == 0) {
			str++;
		} else if ((*str & 0x20) == 0) {
			str += 2;
		} else {
			str += 3;
		}
		counter++;
	}
	return counter;
}

int ZLUnicodeUtil::utf8Length(const std::string &str) {
	return utf8Length(str.data(), str.length());
}

int ZLUnicodeUtil::length(const char *str, int utf8Length) {
	const char *ptr = str;
	for (int i = 0; i < utf8Length; i++) {
		if ((*ptr & 0x80) == 0) {
			ptr++;
		} else if ((*ptr & 0x20) == 0) {
			ptr += 2;
		} else {
			ptr += 3;
		}
	}
	return ptr - str;
}

int ZLUnicodeUtil::length(const std::string &str, int utf8Length) {
	return length(str.data(), utf8Length);
}

void ZLUnicodeUtil::utf8ToUcs2(Ucs2String &to, const char *from, int length, int toLength) {
	to.clear();
	if (toLength < 0) {
		toLength = utf8Length(from, length);
	}
	to.reserve(toLength);
	const char *last = from + length;
	for (const char *ptr = from; ptr < last;) {
		if ((*ptr & 0x80) == 0) {
			to.push_back(*ptr);
			ptr++;
		} else if ((*ptr & 0x20) == 0) {
			Ucs2Char ch = *ptr & 0x1f;
			ptr++;
			ch <<= 6;
			ch += *ptr & 0x3f;
			to.push_back(ch);
			ptr++;
		} else {
			Ucs2Char ch = *ptr & 0x0f;
			ptr++;
			ch <<= 6;
			ch += *ptr & 0x3f;
			ptr++;
			ch <<= 6;
			ch += *ptr & 0x3f;
			to.push_back(ch);
			ptr++;
		}
	}
}

void ZLUnicodeUtil::utf8ToUcs2(Ucs2String &to, const std::string &from, int toLength) {
	utf8ToUcs2(to, from.data(), from.length(), toLength);
}

void ZLUnicodeUtil::ucs2ToUtf8(std::string &to, const Ucs2String &from, int toLength) {
	to.erase();
	if (toLength > 0) {
		to.reserve(toLength);
	}
	for (Ucs2String::const_iterator it = from.begin(); it != from.end(); it++) {
		Ucs2Char ch = *it;
		if (ch < 0x80) {
	    to += (char)ch;
		} else if (ch < 0x800) {
			to += (char)(0xC0 | ch >> 6);
			to += (char)(0x80 | ch & 0x3F);
		} else {
			to += (char)(0xE0 | ch >> 12);
			to += (char)(0x80 | ch >> 6 & 0x3F);
			to += (char)(0x80 | ch & 0x3F);
		}
	}
}

bool ZLUnicodeUtil::isLetter(Ucs2Char ch) {
	return
		(('a' <= ch) && (ch <= 'z')) ||
		(('Z' <= ch) && (ch <= 'Z')) ||
		// ' is "letter" (in French, for example)
		(ch == '\'') ||
		// latin1
		((0xC0 <= ch) && (ch <= 0xFF) && (ch != 0xD7) && (ch != 0xF7)) ||
		// ligatures OE & oe
		(ch == 0x152) || (ch == 0x153) ||
		// &scaron;
		(ch == 0x160) || (ch == 0x161) ||
		// cyrillic
		((0x410 <= ch) && (ch <= 0x44F)) ||
		// cyrillic YO & yo
		(ch == 0x401) || (ch == 0x451);
}

ZLUnicodeUtil::Ucs2Char ZLUnicodeUtil::toLower(Ucs2Char ch) {
	if (('A' <= ch) && (ch <= 'Z')) {
		return ch + 'a' - 'A';
	}

	// latin1
	if ((0xC0 <= ch) && (ch <= 0xDE) && (ch != 0xD7)) {
		return ch + 0x20;
	}

	// ligature oe
	if (ch == 0x152) {
		return 0x153;
	}

	// &scaron;
	if (ch == 0x160) {
		return 0x161;
	}

	// cyrillic
	if ((0x410 <= ch) && (ch <= 0x42F)) {
		return ch + 0x20;
	}

	// cyrillic yo
	if (ch == 0x401) {
		return 0x451;
	}

	return ch;
}

void ZLUnicodeUtil::toLower(Ucs2String &str) {
	for (Ucs2String::iterator it = str.begin(); it != str.end(); it++) {
		*it = toLower(*it);
	}
}

std::string ZLUnicodeUtil::toLower(const std::string &utf8String) {
	Ucs2String ucs2String;
	utf8ToUcs2(ucs2String, utf8String);

	toLower(ucs2String);

	std::string result;
	ucs2ToUtf8(result, ucs2String, utf8String.length());
	return result;
}

ZLUnicodeUtil::Ucs2Char ZLUnicodeUtil::toUpper(Ucs2Char ch) {
	if (('a' <= ch) && (ch <= 'z')) {
		return ch + 'A' - 'a';
	}

	// latin1
	if ((0xE0 <= ch) && (ch <= 0xFE) && (ch != 0xF7)) {
		return ch - 0x20;
	}

	// ligature oe
	if (ch == 0x153) {
		return 0x152;
	}

	// &scaron;
	if (ch == 0x161) {
		return 0x160;
	}

	// cyrillic
	if ((0x430 <= ch) && (ch <= 0x44F)) {
		return ch - 0x20;
	}

	// cyrillic yo
	if (ch == 0x451) {
		return 0x401;
	}

	return ch;
}

void ZLUnicodeUtil::toUpper(Ucs2String &str) {
	for (Ucs2String::iterator it = str.begin(); it != str.end(); it++) {
		*it = toUpper(*it);
	}
}

std::string ZLUnicodeUtil::toUpper(const std::string &utf8String) {
	Ucs2String ucs2String;
	utf8ToUcs2(ucs2String, utf8String);

	toUpper(ucs2String);

	std::string result;
	ucs2ToUtf8(result, ucs2String, utf8String.length());
	return result;
}
