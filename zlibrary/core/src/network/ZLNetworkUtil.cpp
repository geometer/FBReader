/*
 * Copyright (C) 2008-2009 Geometer Plus <contact@geometerplus.com>
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

#include <map>
#include <set>

#include <ZLUnicodeUtil.h>

#include "ZLNetworkUtil.h"

std::string ZLNetworkUtil::hostFromUrl(const std::string &url) {
	std::string host = url;
	int index = host.find("://");
	if (index != -1) {
		host.erase(0, index + 3);
	}
	index = host.find("/");
	if (index != -1) {
		host.erase(index);
	}
	return host;
}

std::string ZLNetworkUtil::htmlEncode(const std::string &stringToEncode) {
	std::string encodedString;
	const char *data = stringToEncode.data();
	const char *end = data + stringToEncode.length();
	while (data < end) {
		ZLUnicodeUtil::Ucs4Char ch;
		int count = ZLUnicodeUtil::firstChar(ch, data);
		if (ZLUnicodeUtil::isLetter(ch) ||
				(ch == '.') ||
				(ch == '~') ||
				(ch == '-') ||
				(ch == '_')) {
			encodedString.append(data, count);
		} else {
			static const char hexDigits[] = "0123456789ABCDEF";
			for (int i = 0; i < count; ++i) {
				encodedString += '%';
				encodedString += hexDigits[((unsigned char)data[i]) / 16];
				encodedString += hexDigits[((unsigned char)data[i]) % 16];
			}
		}
		data += count;
	}
	return encodedString;
}
