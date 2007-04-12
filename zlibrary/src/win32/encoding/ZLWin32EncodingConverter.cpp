/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
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

#include <iconv.h>

#include <ZLUnicodeUtil.h>

#include "ZLWin32EncodingConverter.h"

class ZLWin32EncodingConverter : public ZLEncodingConverter {

private:
	ZLWin32EncodingConverter(const std::string &encoding);
	~ZLWin32EncodingConverter();

	void convert(std::string &dst, const char *srcStart, const char *srcEnd);
	void reset();
	bool fillTable(int *map);

private:
	iconv_t myZLWin32erter;
	std::string myBuffer;

friend class ZLWin32EncodingConverterProvider;
};

bool ZLWin32EncodingConverterProvider::providesConverter(const std::string &encoding) {
	iconv_t converter = iconv_open("utf-8", encoding.c_str());
	if (converter != (iconv_t)-1) {
		iconv_close(converter);
		return true;
	}
	return false;
}

shared_ptr<ZLEncodingConverter> ZLWin32EncodingConverterProvider::createConverter(const std::string &encoding) {
	return new ZLWin32EncodingConverter(encoding);
}

ZLWin32EncodingConverter::ZLWin32EncodingConverter(const std::string &encoding) {
	myZLWin32erter = iconv_open("utf-8", encoding.c_str());
}

ZLWin32EncodingConverter::~ZLWin32EncodingConverter() {
	if (myZLWin32erter != (iconv_t)-1) {
		iconv_close(myZLWin32erter);
	}
}

void ZLWin32EncodingConverter::convert(std::string &dst, const char *srcStart, const char *srcEnd) {
	if ((srcStart == srcEnd) || (myZLWin32erter == (iconv_t)-1)) {
		return;
	}

	size_t inSize;
	char *in;
	if (myBuffer.empty()) {
		inSize = srcEnd - srcStart;
		in = (char*)srcStart;
	} else {
		myBuffer.append(srcStart, srcEnd - srcStart);
		inSize = myBuffer.length();
		in = (char*)myBuffer.data();
	}

	size_t outSize = 3 * inSize;
	const size_t startOutSize = outSize;
	size_t oldLength = dst.length();
	dst.append(outSize, '\0');
	char *out = (char*)dst.data() + oldLength;

iconvlabel:
	iconv(myZLWin32erter, (const char**)&in, &inSize, &out, &outSize);
	if (inSize != 0) {
		if (myBuffer.empty()) {
			myBuffer.append(in, inSize);
		} else {
			myBuffer.erase(0, myBuffer.length() - inSize);
		}
	} else {
		myBuffer.erase();
	}
	if ((myBuffer.length() > 1) && (outSize == startOutSize)) {
		// looks like myBuffer contains incorrect character at start
		myBuffer.erase(0, 1);
		in = (char*)myBuffer.data();
		inSize = myBuffer.length();
		goto iconvlabel;
	}
	dst.erase(oldLength + startOutSize - outSize);
}

void ZLWin32EncodingConverter::reset() {
	myBuffer.erase();
}

bool ZLWin32EncodingConverter::fillTable(int *map) {
	if (myZLWin32erter == (iconv_t)-1) {
		return false;
	}

	char inBuffer[1];
	char outBuffer[3];
	char *in, *out;
	size_t inSize, outSize;
	for (int i = 0; i < 256; ++i) {
		in = inBuffer;
		out = outBuffer;
		inSize = 1;
		outSize = 3;
		inBuffer[0] = i;
		iconv(myZLWin32erter, (const char**)&in, &inSize, &out, &outSize);
		if (inSize == 0) {
			ZLUnicodeUtil::Ucs2Char ch;
			ZLUnicodeUtil::firstChar(ch, outBuffer);
			map[i] = ch;
		} else {
			map[i] = i;
		}
	}
	return true;
}
