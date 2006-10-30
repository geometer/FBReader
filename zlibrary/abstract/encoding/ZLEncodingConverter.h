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

#include <shared_ptr.h>

class ZLEncodingConverter {

public:
	static shared_ptr<ZLEncodingConverter> createConverter(const std::string &encoding);
	static std::string encodingDescriptionPath();
	static std::vector<std::string> &knownEncodings();
	static const std::string &encodingByCode(int code);

private:
	static std::vector<std::string> ourKnownEncodings;
	static std::map<int,std::string> ourEncodingsByCode;

protected:
	ZLEncodingConverter();

public:
	virtual ~ZLEncodingConverter();
	virtual void convert(std::string &dst, const char *srcStart, const char *srcEnd) = 0;
	virtual void reset();
	virtual bool fillTable(int *map) = 0;
};

#endif /* __ZLENCODINGCONVERTER_H__ */
