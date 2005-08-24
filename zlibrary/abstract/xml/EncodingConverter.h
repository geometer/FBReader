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
	EncodingConverter::EncodingConverter(const char *encoding = 0) XML_SECTION;
	EncodingConverter::~EncodingConverter() XML_SECTION;
	void EncodingConverter::setEncoding(const char *encoding) XML_SECTION;
	void EncodingConverter::convert(std::string &dst, const char *srcStart, const char *srcEnd) XML_SECTION;
	void registerExtension(char symbol, const shared_ptr<ControlSequenceExtension> extension) XML_SECTION;

private:
	int *myEncodingMap;
	std::map<char,shared_ptr<ControlSequenceExtension> > myExtensions;
	shared_ptr<ControlSequenceExtension> myActiveExtension;
};

inline ControlSequenceExtension::ControlSequenceExtension() {}
inline ControlSequenceExtension::~ControlSequenceExtension() {}

#endif /* __ENCODINGCONVERTER_H__ */
