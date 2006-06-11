/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <abstract/ZLUnicodeUtil.h>

#include "ZLEncodingConverter.h"
#include "EncodingConverters.h"
#include "EncodingReader.h"

shared_ptr<ZLEncodingConverter> ZLEncodingConverter::createConverter(const std::string &encoding) {
	if (encoding.empty() ||
			strcasecmp(encoding.c_str(), "UTF-8") ||
			strcasecmp(encoding.c_str(), "US-ASCII")) {
		return new DummyEncodingConverter();
	}

	const std::vector<std::string> &encodingList = knownEncodings();
	std::vector<std::string>::const_iterator it;
	for (it = encodingList.begin(); it != encodingList.end(); it++) {
		if (strcasecmp(encoding.c_str(), it->c_str()) == 0) {
			break;
		}
	}

	if (it != encodingList.end()) {
		EncodingCharReader er(*it);
		char **encodingMap = er.createTable();
		if (encodingMap != 0) {
			if (er.bytesNumber() == 1) {
				return new OneByteEncodingConverter(*it, encodingMap);
			} else if (er.bytesNumber() == 2) {
				return new TwoBytesEncodingConverter(encodingMap);
			}
		}
	}

	IconvEncodingConverter *converter = new IconvEncodingConverter(encoding);
	if (converter->isInitialized()) {
		return converter;
	} else {
		delete converter;
	}

	return new DummyEncodingConverter();
}

ZLEncodingConverter::ZLEncodingConverter() {
}

ZLEncodingConverter::~ZLEncodingConverter() {
}

void ZLEncodingConverter::reset() {
}

DummyEncodingConverter::DummyEncodingConverter() {
}

DummyEncodingConverter::~DummyEncodingConverter() {
}

void DummyEncodingConverter::convert(std::string &dst, const char *srcStart, const char *srcEnd) {
	dst.append(srcStart, srcEnd - srcStart);
}

bool DummyEncodingConverter::fillTable(int *map) {
	for (int i = 0; i < 255; i++) {
		map[i] = i;
	}
	return true;
}

OneByteEncodingConverter::OneByteEncodingConverter(const std::string &encoding, char **encodingMap) : myEncoding(encoding) {
	myEncodingMap = new char[1024];
	memset(myEncodingMap, '\0', 1024);
	for (int i = 0; i < 256; i++) {
		myEncodingMap[4 * i] = i;
	}
	if (encodingMap != 0) {
		for (int i = 0; i < 256; i++) {
			if (encodingMap[i] != 0) {
				strcpy(myEncodingMap + 4 * i, encodingMap[i]);
			}
		}
	}
}

OneByteEncodingConverter::~OneByteEncodingConverter() {
	delete[] myEncodingMap;
}

void OneByteEncodingConverter::convert(std::string &dst, const char *srcStart, const char *srcEnd) {
	size_t oldLength = dst.length();
	dst.append(3 * (srcEnd - srcStart), '\0');
	char *dstStartPtr = (char*)dst.data() + oldLength;
	char *dstPtr = dstStartPtr;
	const char *p;
	for (const char *ptr = srcStart; ptr != srcEnd; ptr++) {
		for (p = myEncodingMap + 4 * (unsigned char)*ptr; *p != '\0'; p++) {
			(*dstPtr++) = *p;
		}
	}
	//dst.erase(dstPtr - dstStartPtr + oldLength);
}

bool OneByteEncodingConverter::fillTable(int *map) {
	return EncodingIntReader(myEncoding).fillTable(map);
}

TwoBytesEncodingConverter::TwoBytesEncodingConverter(char **encodingMap) : myEncodingMap(encodingMap), myLastCharIsNotProcessed(false) {
}

TwoBytesEncodingConverter::~TwoBytesEncodingConverter() {
	for (int i = 0; i < 32768; i++) {
		if (myEncodingMap[i] != 0) {
			delete[] myEncodingMap[i];
		}
	}
	delete[] myEncodingMap;
}

void TwoBytesEncodingConverter::convert(std::string &dst, const char *srcStart, const char *srcEnd) {
	if (srcStart == srcEnd) {
		return;
	}

	dst.reserve(dst.length() + 3 * (srcEnd - srcStart) / 2);
	if (myLastCharIsNotProcessed) {
		const char *utf8 = myEncodingMap[0x100 * (myLastChar & 0x7F) + (unsigned char)*srcStart];
		if (utf8 != 0) {
			dst += utf8;
		}
		srcStart++;
		myLastCharIsNotProcessed = false;
	}
	for (const char *ptr = srcStart; ptr != srcEnd; ptr++) {
		if (((*ptr) & 0x80) == 0) {
			dst += *ptr;
		} else if (ptr + 1 == srcEnd) {
			myLastChar = *ptr;
			myLastCharIsNotProcessed = true;
		} else {
			const char *utf8 = myEncodingMap[0x100 * ((*ptr) & 0x7F) + (unsigned char)*(ptr + 1)];
			if (utf8 != 0) {
				dst += utf8;
			}
			ptr++;
		}
	}
}

void TwoBytesEncodingConverter::reset() {
	myLastCharIsNotProcessed = false;
}

bool TwoBytesEncodingConverter::fillTable(int*) {
	return false;
}

IconvEncodingConverter::IconvEncodingConverter(const std::string &encoding) {
	myIConverter = iconv_open("utf-8", encoding.c_str());
}

IconvEncodingConverter::~IconvEncodingConverter() {
	if (myIConverter != (iconv_t)-1) {
		iconv_close(myIConverter);
	}
}

void IconvEncodingConverter::convert(std::string &dst, const char *srcStart, const char *srcEnd) {
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
	//dst.erase(oldLength + startOutSize - outSize);
}

void IconvEncodingConverter::reset() {
	myBuffer.erase();
}

bool IconvEncodingConverter::isInitialized() const {
	return myIConverter != (iconv_t)-1;
}

bool IconvEncodingConverter::fillTable(int *map) {
	if (!isInitialized()) {
		return false;
	}

	char inBuffer[1];
	char outBuffer[3];
	char *in, *out;
	size_t inSize, outSize;
	for (int i = 0; i < 256; i++) {
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

const std::string &ZLEncodingConverter::encodingByCode(int code) {
	static const std::string EMPTY;
	std::map<int,std::string>::const_iterator it = ourEncodingsByCode.find(code);
	return (it != ourEncodingsByCode.end()) ? it->second : EMPTY;
}
