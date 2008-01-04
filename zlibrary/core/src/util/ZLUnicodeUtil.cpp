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

#include "ZLUnicodeUtil.h"

bool ZLUnicodeUtil::isUtf8String(const char *str, int len) {
	const char *last = str + len;
	int nonLeadingCharsCounter = 0;
	for (; str < last; ++str) {
		if (nonLeadingCharsCounter == 0) {
			if ((*str & 0x80) != 0) {
				if ((*str & 0xE0) == 0xC0) {
					nonLeadingCharsCounter = 1;
				} else if ((*str & 0xF0) == 0xE0) {
					nonLeadingCharsCounter = 2;
				} else if ((*str & 0xF8) == 0xF0) {
					nonLeadingCharsCounter = 3;
				} else {
					return false;
				}
			}
		} else {
			if ((*str & 0xC0) != 0x80) {
				return false;
			}
			--nonLeadingCharsCounter;
		}
	}
	return nonLeadingCharsCounter == 0;
}

bool ZLUnicodeUtil::isUtf8String(const std::string &str) {
	return isUtf8String(str.data(), str.length());
}

int ZLUnicodeUtil::utf8Length(const char *str, int len) {
	const char *last = str + len;
	int counter = 0;
	while (str < last) {
		if ((*str & 0x80) == 0) {
			++str;
		} else if ((*str & 0x20) == 0) {
			str += 2;
		} else if ((*str & 0x10) == 0) {
			str += 3;
		} else {
			str += 4;
		}
		++counter;
	}
	return counter;
}

int ZLUnicodeUtil::utf8Length(const std::string &str) {
	return utf8Length(str.data(), str.length());
}

int ZLUnicodeUtil::length(const char *str, int utf8Length) {
	const char *ptr = str;
	for (int i = 0; i < utf8Length; ++i) {
		if ((*ptr & 0x80) == 0) {
			++ptr;
		} else if ((*ptr & 0x20) == 0) {
			ptr += 2;
		} else if ((*ptr & 0x10) == 0) {
			ptr += 3;
		} else {
			ptr += 4;
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
			++ptr;
		} else if ((*ptr & 0x20) == 0) {
			Ucs2Char ch = *ptr & 0x1f;
			++ptr;
			ch <<= 6;
			ch += *ptr & 0x3f;
			to.push_back(ch);
			++ptr;
		} else if ((*ptr & 0x10) == 0) {
			Ucs2Char ch = *ptr & 0x0f;
			++ptr;
			ch <<= 6;
			ch += *ptr & 0x3f;
			++ptr;
			ch <<= 6;
			ch += *ptr & 0x3f;
			to.push_back(ch);
			++ptr;
		} else {
			// symbol number is > 0xffff :(
			to.push_back('X');
			ptr += 4;
		}
	}
}

void ZLUnicodeUtil::utf8ToUcs2(Ucs2String &to, const std::string &from, int toLength) {
	utf8ToUcs2(to, from.data(), from.length(), toLength);
}

int ZLUnicodeUtil::firstChar(Ucs2Char &ch, const char *utf8String) {
	if ((*utf8String & 0x80) == 0) {
		ch = *utf8String;
		return 1;
	} else if ((*utf8String & 0x20) == 0) {
		ch = *utf8String & 0x1f;
		ch <<= 6;
		ch += *(utf8String + 1) & 0x3f;
		return 2;
	} else {
		ch = *utf8String & 0x0f;
		ch <<= 6;
		ch += *(utf8String + 1) & 0x3f;
		ch <<= 6;
		ch += *(utf8String + 2) & 0x3f;
		return 3;
	}
}

int ZLUnicodeUtil::ucs2ToUtf8(char *to, Ucs2Char ch) {
	if (ch < 0x80) {
		*to = (char)ch;
		return 1;
	} else if (ch < 0x800) {
		*to = (char)(0xC0 | (ch >> 6));
		*(to + 1) = (char)(0x80 | (ch & 0x3F));
		return 2;
	} else {
		*to = (char)(0xE0 | ch >> 12);
		*(to + 1) = (char)(0x80 | ((ch >> 6) & 0x3F));
		*(to + 2) = (char)(0x80 | (ch & 0x3F));
		return 3;
	}
}

void ZLUnicodeUtil::ucs2ToUtf8(std::string &to, const Ucs2String &from, int toLength) {
	char buffer[3];
	to.erase();
	if (toLength > 0) {
		to.reserve(toLength);
	}
	for (Ucs2String::const_iterator it = from.begin(); it != from.end(); ++it) {
		to.append(buffer, ucs2ToUtf8(buffer, *it));
	}
}

bool ZLUnicodeUtil::isLetter(Ucs2Char ch) {
	return
		(('a' <= ch) && (ch <= 'z')) ||
		(('A' <= ch) && (ch <= 'Z')) ||
		// ' is "letter" (in French, for example)
		(ch == '\'') ||
		// ^ is "letter" (in Esperanto)
		(ch == '^') ||
		// latin1
		((0xC0 <= ch) && (ch <= 0xFF) && (ch != 0xD7) && (ch != 0xF7)) ||
		// extended latin1
		((0x100 <= ch) && (ch <= 0x178)) ||
		// cyrillic
		((0x410 <= ch) && (ch <= 0x44F)) ||
		// cyrillic YO & yo
		(ch == 0x401) || (ch == 0x451);
}

bool ZLUnicodeUtil::isSpace(Ucs2Char ch) {
	return
		((9 <= ch) && (ch <= 13)) ||
		(ch == 32) ||
		//(ch == 160) ||
		(ch == 5760) ||
		((8192 <= ch) && (ch <= 8203)) ||
		(ch == 8232) ||
		(ch == 8233) ||
		(ch == 8239) ||
		(ch == 8287) ||
		(ch == 12288);
}

ZLUnicodeUtil::Breakable ZLUnicodeUtil::isBreakable(Ucs2Char c) {
	if (c <= 0x2000) {
		return NO_BREAKABLE;
	}

	if (((c < 0x2000) || (c > 0x2006)) &&
			((c < 0x2008) || (c > 0x2046)) &&
			((c < 0x207D) || (c > 0x207E)) &&
			((c < 0x208D) || (c > 0x208E)) &&
			((c < 0x2329) || (c > 0x232A)) &&
			((c < 0x3001) || (c > 0x3003)) &&
			((c < 0x3008) || (c > 0x3011)) &&
			((c < 0x3014) || (c > 0x301F)) &&
			((c < 0xFD3E) || (c > 0xFD3F)) &&
			((c < 0xFE30) || (c > 0xFE44)) &&
			((c < 0xFE49) || (c > 0xFE52)) &&
			((c < 0xFE54) || (c > 0xFE61)) &&
			((c < 0xFE6A) || (c > 0xFE6B)) &&
			((c < 0xFF01) || (c > 0xFF03)) &&
			((c < 0xFF05) || (c > 0xFF0A)) &&
			((c < 0xFF0C) || (c > 0xFF0F)) &&
			((c < 0xFF1A) || (c > 0xFF1B)) &&
			((c < 0xFF1F) || (c > 0xFF20)) &&
			((c < 0xFF3B) || (c > 0xFF3D)) &&
			((c < 0xFF61) || (c > 0xFF65)) &&
			(c != 0xFE63) &&
			(c != 0xFE68) &&
			(c != 0x3030) &&
			(c != 0x30FB) &&
			(c != 0xFF3F) &&
			(c != 0xFF5B) &&
			(c != 0xFF5D)) {
		return NO_BREAKABLE;
	}

	if (((c >= 0x201A) && (c <= 0x201C)) ||
			((c >= 0x201E) && (c <= 0x201F))) { 
		return BREAKABLE_BEFORE; 
	}
	switch (c) { 
		case 0x2018: case 0x2039: case 0x2045: 
		case 0x207D: case 0x208D: case 0x2329: 
		case 0x3008: case 0x300A: case 0x300C: 
		case 0x300E: case 0x3010: case 0x3014: 
		case 0x3016: case 0x3018: case 0x301A: 
		case 0x301D: case 0xFD3E: case 0xFE35: 
		case 0xFE37: case 0xFE39: case 0xFE3B: 
		case 0xFE3D: case 0xFE3F: case 0xFE41: 
		case 0xFE43: case 0xFE59: case 0xFE5B: 
		case 0xFE5D: case 0xFF08: case 0xFF3B: 
		case 0xFF5B: case 0xFF62: 
			return BREAKABLE_BEFORE; 
	} 
	return BREAKABLE_AFTER;
}

ZLUnicodeUtil::Ucs2Char ZLUnicodeUtil::toLower(Ucs2Char ch) {
	if (('A' <= ch) && (ch <= 'Z')) {
		return ch + 'a' - 'A';
	}

	// latin1
	if ((0xC0 <= ch) && (ch <= 0xDE) && (ch != 0xD7)) {
		return ch + 0x20;
	}

	// extended latin1
	if ((0x100 <= ch) && (ch <= 0x178)) {
		return (ch & 0x01) ? ch : (ch + 1);
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
	for (Ucs2String::iterator it = str.begin(); it != str.end(); ++it) {
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

	// extended latin1
	if ((0x100 <= ch) && (ch <= 0x178)) {
		return (ch & 0x01) ? (ch - 1) : ch;
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
	for (Ucs2String::iterator it = str.begin(); it != str.end(); ++it) {
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
