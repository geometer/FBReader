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

#ifndef __ENCODINGCONVERTER_H__
#define __ENCODINGCONVERTER_H__

#include <string>
#include <map>

#include <abstract/shared_ptr.h>

class ControlSequenceExtension {

public:
	ControlSequenceExtension() XML_SECTION;
	virtual ~ControlSequenceExtension() XML_SECTION;

	virtual void start() XML_SECTION = 0;
	virtual bool parseCharacter(char ch) XML_SECTION = 0;
	virtual const std::string &buffer() const XML_SECTION = 0;
};

class EncodingConverter {

public:
	static shared_ptr<EncodingConverter> createConverter(const std::string &encoding) XML_SECTION;

protected:
	EncodingConverter() XML_SECTION;

public:
	virtual ~EncodingConverter() XML_SECTION;
	virtual void convert(std::string &dst, const char *srcStart, const char *srcEnd) XML_SECTION = 0;
	void registerExtension(char symbol, const shared_ptr<ControlSequenceExtension> extension) XML_SECTION;

protected:
	shared_ptr<ControlSequenceExtension> myExtensions[256];
	int myExtensionNumber;
	shared_ptr<ControlSequenceExtension> myActiveExtension;
};

class DummyEncodingConverter : public EncodingConverter {

private:
	DummyEncodingConverter() XML_SECTION;

public:
	~DummyEncodingConverter() XML_SECTION;
	void convert(std::string &dst, const char *srcStart, const char *srcEnd) XML_SECTION;

friend class EncodingConverter;
};

class OneByteEncodingConverter : public EncodingConverter {

private:
	OneByteEncodingConverter(char **encodingMap) XML_SECTION;

public:
	~OneByteEncodingConverter() XML_SECTION;
	void convert(std::string &dst, const char *srcStart, const char *srcEnd) XML_SECTION;

private:
	void setDummyEncoding() XML_SECTION;

private:
	char **myEncodingMap;

friend class EncodingConverter;
};

inline ControlSequenceExtension::ControlSequenceExtension() {}
inline ControlSequenceExtension::~ControlSequenceExtension() {}

#endif /* __ENCODINGCONVERTER_H__ */
