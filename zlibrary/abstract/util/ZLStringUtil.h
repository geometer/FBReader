/*
 * Copyright (C) 2005 Nikolay Pultsin <geometer@mawhrin.net>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __ZLSTRINGUTIL_H__
#define __ZLSTRINGUTIL_H__

#include <vector>
#include <string>

class ZLLowMemoryString;

class ZLStringUtil {

private:
	ZLStringUtil() UTIL_SECTION;

public:
	static void append(ZLLowMemoryString &target, const std::vector<ZLLowMemoryString> &text) UTIL_SECTION;
	static bool stringEndsWith(const std::string &str, const std::string &end) UTIL_SECTION;
	static void appendNumber(std::string &str, unsigned int n) UTIL_SECTION;
};

#endif /* __ZLSTRINGUTIL_H__ */
