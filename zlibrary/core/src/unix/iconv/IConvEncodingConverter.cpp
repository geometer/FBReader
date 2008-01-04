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

#include <iconv.h>

#include <ZLUnicodeUtil.h>

#include "IConvEncodingConverter.h"

class IConvEncodingConverter : public ZLEncodingConverter {

private:
	IConvEncodingConverter(const std::string &encoding);
	~IConvEncodingConverter();

	void convert(std::string &dst, const char *srcStart, const char *srcEnd);
	void reset();
	bool fillTable(int *map);

private:
	iconv_t myIConverter;
	std::string myBuffer;

friend class IConvEncodingConverterProvider;
};

bool IConvEncodingConverterProvider::providesConverter(const std::string &encoding) {
	iconv_t converter = iconv_open("utf-8", encoding.c_str());
	if (converter != (iconv_t)-1) {
		iconv_close(converter);
		return true;
	}
	return false;
}

shared_ptr<ZLEncodingConverter> IConvEncodingConverterProvider::createConverter(const std::string &encoding) {
	return new IConvEncodingConverter(encoding);
}

IConvEncodingConverter::IConvEncodingConverter(const std::string &encoding) {
	myIConverter = iconv_open("utf-8", encoding.c_str());
}

IConvEncodingConverter::~IConvEncodingConverter() {
	if (myIConverter != (iconv_t)-1) {
		iconv_close(myIConverter);
	}
}

void IConvEncodingConverter::convert(std::string &dst, const char *srcStart, const char *srcEnd) {
	if ((srcStart == srcEnd) || (myIConverter == (iconv_t)-1)) {
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
	iconv(myIConverter, &in, &inSize, &out, &outSize);
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

void IConvEncodingConverter::reset() {
	myBuffer.erase();
}

bool IConvEncodingConverter::fillTable(int *map) {
	if (myIConverter == (iconv_t)-1) {
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
		iconv(myIConverter, &in, &inSize, &out, &outSize);
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
