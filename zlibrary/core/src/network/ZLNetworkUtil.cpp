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

#include <ZLStringUtil.h>
#include <ZLUnicodeUtil.h>

#include "ZLNetworkUtil.h"

std::string ZLNetworkUtil::hostFromUrl(const std::string &url) {
	std::string host = url;
	size_t index = host.find("://");
	if (index != std::string::npos) {
		host.erase(0, index + 3);
	}
	index = host.find("/");
	if (index != std::string::npos) {
		host.erase(index);
	}
	return host;
}

std::string ZLNetworkUtil::url(const std::string &baseUrl, const std::string &relativePath) {
	if (relativePath.empty()) {
		return relativePath;
	}

	size_t index = relativePath.find("://");
	if (index != std::string::npos) {
		return relativePath;
	}

	if (relativePath[0] == '/') {
		index = baseUrl.find("://");
		index = baseUrl.find("/", index + 3);
		return baseUrl.substr(0, index) + relativePath;
	}

	index = baseUrl.rfind('/');
	return baseUrl.substr(0, index + 1) + relativePath;
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

void ZLNetworkUtil::addParameter(std::string &url, const std::string &name, const std::string &value) {
	std::string val(value);
	ZLStringUtil::stripWhiteSpaces(val);
	if (!val.empty()) {
		if (url.find('?') == std::string::npos) {
			url.append("?");
		} else {
			url.append("&");
		}
		url.append(name);
		url.append("=");
		url.append(ZLNetworkUtil::htmlEncode(val));
	}
}
