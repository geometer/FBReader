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

#ifndef __ZLENCODINGCONVERTER_H__
#define __ZLENCODINGCONVERTER_H__

#include <string>
#include <vector>
#include <map>

#include <abstract/shared_ptr.h>

class ZLEncodingConverter {

public:
	static shared_ptr<ZLEncodingConverter> createConverter(const std::string &encoding) XML_SECTION;
	static shared_ptr<ZLEncodingConverter> createConverter(int encoding) XML_SECTION;
	static void setEncodingDescriptionPath(const std::string &path) XML_SECTION;
	static std::string &encodingDescriptionPath() XML_SECTION;
	static std::vector<std::string> &knownEncodings() XML_SECTION;

private:
	static std::string ourEncodingDescriptionPath;
	static std::vector<std::string> ourKnownEncodings;
	static std::map<int,std::string> ourEncodingsByCode;

protected:
	ZLEncodingConverter() XML_SECTION;

public:
	virtual ~ZLEncodingConverter() XML_SECTION;
	virtual void convert(std::string &dst, const char *srcStart, const char *srcEnd) XML_SECTION = 0;
	virtual void reset() XML_SECTION;
	virtual bool fillTable(int *map) XML_SECTION = 0;
};

inline std::string &ZLEncodingConverter::encodingDescriptionPath() { return ourEncodingDescriptionPath; }
inline std::vector<std::string> &ZLEncodingConverter::knownEncodings() { return ourKnownEncodings; }

#endif /* __ZLENCODINGCONVERTER_H__ */
