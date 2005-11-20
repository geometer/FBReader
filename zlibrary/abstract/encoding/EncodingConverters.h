/*
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#ifndef __ENCODINGCONVERTERS_H__
#define __ENCODINGCONVERTERS_H__

#include <iconv.h>

#include "ZLEncodingConverter.h"

class DummyEncodingConverter : public ZLEncodingConverter {

private:
	DummyEncodingConverter() XML_SECTION;

public:
	~DummyEncodingConverter() XML_SECTION;
	void convert(std::string &dst, const char *srcStart, const char *srcEnd) XML_SECTION;
	bool fillTable(int *map) XML_SECTION;

friend class ZLEncodingConverter;
};

class OneByteEncodingConverter : public ZLEncodingConverter {

private:
	OneByteEncodingConverter(const std::string &encoding, char **encodingMap) XML_SECTION;

public:
	~OneByteEncodingConverter() XML_SECTION;
	void convert(std::string &dst, const char *srcStart, const char *srcEnd) XML_SECTION;
	bool fillTable(int *map) XML_SECTION;

private:
	const std::string myEncoding;
	char **myEncodingMap;

friend class ZLEncodingConverter;
};

class TwoBytesEncodingConverter : public ZLEncodingConverter {

private:
	TwoBytesEncodingConverter(char **encodingMap) XML_SECTION;

public:
	~TwoBytesEncodingConverter() XML_SECTION;
	void convert(std::string &dst, const char *srcStart, const char *srcEnd) XML_SECTION;
	void reset() XML_SECTION;
	bool fillTable(int *map) XML_SECTION;

private:
	char **myEncodingMap;
	
	char myLastChar;
	bool myLastCharIsNotProcessed;

friend class ZLEncodingConverter;
};

class IconvEncodingConverter : public ZLEncodingConverter {

private:
	IconvEncodingConverter(const std::string &encoding) XML_SECTION;

public:
	~IconvEncodingConverter() XML_SECTION;
	void convert(std::string &dst, const char *srcStart, const char *srcEnd) XML_SECTION;
	void reset() XML_SECTION;
	bool isInitialized() const XML_SECTION;
	bool fillTable(int *map) XML_SECTION;

private:
	iconv_t myIConverter;
	std::string myBuffer;

friend class ZLEncodingConverter;
};

#endif /* __ENCODINGCONVERTERS_H__ */
