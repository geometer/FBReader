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

#include <windows.h>

#include <ZLUnicodeUtil.h>

#include "ZLWin32EncodingConverter.h"

class ZLWin32EncodingConverter : public ZLEncodingConverter {

private:
	ZLWin32EncodingConverter(UINT code);
	~ZLWin32EncodingConverter();

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

bool ZLWin32EncodingConverterProvider::providesConverter(const std::string &encoding) {
	UINT code = atoi(encoding.c_str());
	if (code == 0) {
		return false;
	}

	static const std::string TEST = "TEST";
	int len = TEST.length();
	ZLUnicodeUtil::Ucs2String ucs2String;
	ucs2String.insert(ucs2String.end(), len, 0);
	return MultiByteToWideChar(code, MB_PRECOMPOSED, TEST.c_str(), len, (WCHAR*)&ucs2String.front(), len) == len;
}

shared_ptr<ZLEncodingConverter> ZLWin32EncodingConverterProvider::createConverter(const std::string &encoding) {
	return new ZLWin32EncodingConverter(atoi(encoding.c_str()));
}

ZLWin32EncodingConverter::ZLWin32EncodingConverter(UINT code) : myCode(code), myUseStoredCharacter(false) {
}

ZLWin32EncodingConverter::~ZLWin32EncodingConverter() {
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
		if (MultiByteToWideChar(myCode, MB_PRECOMPOSED, buf, 2, &symbol, 1) == 1) {
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
		int ucs2Len = MultiByteToWideChar(myCode, MB_PRECOMPOSED, srcStart, len, bufferStart, len);
		myBuffer.erase(myBuffer.begin() + ucs2Len + (hasFirstChar ? 1 : 0), myBuffer.end());

		if (ucs2Len != len) {
			myRBuffer.append(len, '\0');
			char defaultChar = 'X';
			BOOL usedDefaultChar = false;
			int len1 = WideCharToMultiByte(myCode, MB_PRECOMPOSED, bufferStart, ucs2Len, (char*)myRBuffer.data(), len, &defaultChar, &usedDefaultChar);
			if (len1 == len - 1) {
				myUseStoredCharacter = true;
				myStoredCharacter = *(srcEnd - 1);
			}
			myRBuffer.erase();
		}
	}

	ZLUnicodeUtil::ucs2ToUtf8(dst, myBuffer);
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
		if (MultiByteToWideChar(myCode, MB_PRECOMPOSED, &in, 1, &out, 1) == 1) {
			map[i] = out;
		} else {
			map[i] = i;
		}
	}
	return true;
}
