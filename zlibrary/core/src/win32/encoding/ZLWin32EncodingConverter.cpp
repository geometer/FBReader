/*
 * Copyright (C) 2004-2010 Geometer Plus <contact@geometerplus.com>
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

#include <windows.h>

#include <ZLUnicodeUtil.h>

#include "ZLWin32EncodingConverter.h"

class ZLWin32EncodingConverter : public ZLEncodingConverter {

private:
	ZLWin32EncodingConverter(UINT code);

	void convert(std::string &dst, const char *srcStart, const char *srcEnd);
	void reset();
	bool fillTable(int *map);

private:
	UINT myCode;
	ZLUnicodeUtil::Ucs2String myBuffer;
	std::string myRBuffer;
	bool myUseStoredCharacter;
	char myStoredCharacter;

friend class ZLWin32EncodingConverterProvider;
};

class ZLWin32Utf16EncodingConverter : public ZLEncodingConverter {

private:
	ZLWin32Utf16EncodingConverter(bool littleEndian);

	void convert(std::string &dst, const char *srcStart, const char *srcEnd);
	void reset();
	bool fillTable(int *map);

private:
	bool myLittleEndian;
	ZLUnicodeUtil::Ucs2String myBuffer;
	bool myUseStoredCharacter;
	unsigned char myStoredCharacter;

friend class ZLWin32EncodingConverterProvider;
};

bool ZLWin32EncodingConverterProvider::providesConverter(const std::string &encoding) {
	if (encoding == "UTF-16") {
		return true;
	}
	if (encoding == "UTF-16BE") {
		return true;
	}

	UINT code = atoi(encoding.c_str());
	if (code == 0) {
		return false;
	}

	static const std::string TEST = "TEST";
	int len = TEST.length();
	return MultiByteToWideChar(code, 0, TEST.c_str(), len, 0, 0) == len;
}

shared_ptr<ZLEncodingConverter> ZLWin32EncodingConverterProvider::createConverter(const std::string &encoding) {
	if (encoding == "UTF-16") {
		return new ZLWin32Utf16EncodingConverter(true);
	}
	if (encoding == "UTF-16BE") {
		return new ZLWin32Utf16EncodingConverter(false);
	}
	return new ZLWin32EncodingConverter(atoi(encoding.c_str()));
}

ZLWin32EncodingConverter::ZLWin32EncodingConverter(UINT code) : myCode(code), myUseStoredCharacter(false) {
}

void ZLWin32EncodingConverter::convert(std::string &dst, const char *srcStart, const char *srcEnd) {
	if ((srcStart == srcEnd) || (myCode == 0)) {
		return;
	}

	myBuffer.clear();

	bool hasFirstChar = false;
	if (myUseStoredCharacter) {
		WCHAR symbol;
		char buf[2] = { myStoredCharacter, *srcStart };
		if (MultiByteToWideChar(myCode, 0, buf, 2, &symbol, 1) == 1) {
			hasFirstChar = true;
			myBuffer.push_back(symbol);
			srcStart++;
		}
		myUseStoredCharacter = false;
	}

	int len = srcEnd - srcStart;
	if (len > 0) {
		myBuffer.insert(myBuffer.end(), len, 0);
		WCHAR *bufferStart = (WCHAR*)&myBuffer.front();
		if (hasFirstChar) {
			bufferStart++;
		}
		int ucs2Len = MultiByteToWideChar(myCode, 0, srcStart, len, bufferStart, len);
		myBuffer.erase(myBuffer.begin() + ucs2Len + (hasFirstChar ? 1 : 0), myBuffer.end());

		if (ucs2Len != len) {
			myRBuffer.append(len, '\0');
			char defaultChar = 'X';
			BOOL usedDefaultChar = false;
			int len1 = WideCharToMultiByte(myCode, 0, bufferStart, ucs2Len, (char*)myRBuffer.data(), len, &defaultChar, &usedDefaultChar);
			if (len1 == len - 1) {
				myUseStoredCharacter = true;
				myStoredCharacter = *(srcEnd - 1);
			}
			myRBuffer.erase();
		}
	}

	std::string toAppend;
	ZLUnicodeUtil::ucs2ToUtf8(toAppend, myBuffer);
	dst += toAppend;
	myBuffer.clear();
}

void ZLWin32EncodingConverter::reset() {
	myUseStoredCharacter = false;
}

bool ZLWin32EncodingConverter::fillTable(int *map) {
	if (myCode == 0) {
		return false;
	}

	char in;
	WCHAR out;
	for (int i = 0; i < 256; ++i) {
		in = i;
		if (MultiByteToWideChar(myCode, 0, &in, 1, &out, 1) == 1) {
			map[i] = out;
		} else {
			map[i] = i;
		}
	}
	return true;
}

ZLWin32Utf16EncodingConverter::ZLWin32Utf16EncodingConverter(bool littleEndian) : myLittleEndian(littleEndian), myUseStoredCharacter(false) {
}

void ZLWin32Utf16EncodingConverter::convert(std::string &dst, const char *srcStart, const char *srcEnd) {
	if (srcStart == srcEnd) {
		return;
	}

	const unsigned char *ptr = (const unsigned char*)srcStart;
	ZLUnicodeUtil::Ucs2Char ch;

	myBuffer.clear();

	if (myUseStoredCharacter) {
		if (myLittleEndian) {
			ch = *ptr;
			ch <<= 8;
			ch += myStoredCharacter;
		} else {
			ch = myStoredCharacter;
			ch <<= 8;
			ch += *ptr;
		}
		myBuffer.push_back(ch);
		++ptr;
	}

	const unsigned char *end = (const unsigned char*)srcEnd;
	if ((end - ptr) % 2 == 1) {
		myStoredCharacter = *end;
		myUseStoredCharacter = true;
		--end;
	} else {
		myUseStoredCharacter = false;
	}

	if (myLittleEndian) {
		for (; ptr < end; ptr += 2) {
			myBuffer.push_back((((ZLUnicodeUtil::Ucs2Char)*(ptr + 1)) << 8) + *ptr);
		}
	} else {
		for (; ptr < end; ptr += 2) {
			myBuffer.push_back((((ZLUnicodeUtil::Ucs2Char)*ptr) << 8) + *(ptr + 1));
		}
	}

	std::string toAppend;
	ZLUnicodeUtil::ucs2ToUtf8(toAppend, myBuffer);
	dst += toAppend;
	myBuffer.clear();
}

void ZLWin32Utf16EncodingConverter::reset() {
	myUseStoredCharacter = false;
}

bool ZLWin32Utf16EncodingConverter::fillTable(int *map) {
	return false;
}
