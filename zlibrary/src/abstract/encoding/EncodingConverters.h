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

#ifndef __ENCODINGCONVERTERS_H__
#define __ENCODINGCONVERTERS_H__

#include <iconv.h>

#include "ZLEncodingConverter.h"

class DummyEncodingConverter : public ZLEncodingConverter {

private:
	DummyEncodingConverter();

public:
	~DummyEncodingConverter();
	void convert(std::string &dst, const char *srcStart, const char *srcEnd);
	bool fillTable(int *map);

friend class ZLEncodingConverter;
};

class OneByteEncodingConverter : public ZLEncodingConverter {

private:
	OneByteEncodingConverter(const std::string &encoding, char **encodingMap);

public:
	~OneByteEncodingConverter();
	void convert(std::string &dst, const char *srcStart, const char *srcEnd);
	bool fillTable(int *map);

private:
	const std::string myEncoding;
	char *myEncodingMap;

friend class ZLEncodingConverter;
};

class TwoBytesEncodingConverter : public ZLEncodingConverter {

private:
	TwoBytesEncodingConverter(char **encodingMap);

public:
	~TwoBytesEncodingConverter();
	void convert(std::string &dst, const char *srcStart, const char *srcEnd);
	void reset();
	bool fillTable(int *map);

private:
	char **myEncodingMap;
	
	char myLastChar;
	bool myLastCharIsNotProcessed;

friend class ZLEncodingConverter;
};

class IconvEncodingConverter : public ZLEncodingConverter {

private:
	IconvEncodingConverter(const std::string &encoding);

public:
	~IconvEncodingConverter();
	void convert(std::string &dst, const char *srcStart, const char *srcEnd);
	void reset();
	bool isInitialized() const;
	bool fillTable(int *map);

private:
	iconv_t myIConverter;
	std::string myBuffer;

friend class ZLEncodingConverter;
};

#endif /* __ENCODINGCONVERTERS_H__ */
