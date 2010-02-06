/*
 * Copyright (C) 2008-2010 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLNETWORKUTIL_H__
#define __ZLNETWORKUTIL_H__

#include <string>

class ZLNetworkUtil {

public:
	static std::string htmlEncode(const std::string &stringToEncode);
	static std::string hostFromUrl(const std::string &url);
	static std::string url(const std::string &baseUrl, const std::string &relativePath);

	static std::string &appendParameter(std::string &url, const std::string &name, const std::string &value);
	static bool hasParameter(const std::string &url, const std::string &name);

private:
	ZLNetworkUtil();
};

#endif /* __ZLNETWORKUTIL_H__ */
