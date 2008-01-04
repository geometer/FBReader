/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <stdlib.h>

#include <ZLApplication.h>
#include <ZLFile.h>
#include <ZLStringUtil.h>

#include "MiscUtil.h"

bool MiscUtil::isReference(const std::string &text) {
	return
		ZLStringUtil::stringStartsWith(text, "http://") ||
		ZLStringUtil::stringStartsWith(text, "https://") ||
		ZLStringUtil::stringStartsWith(text, "mailto:") ||
		ZLStringUtil::stringStartsWith(text, "ftp://");
}

std::string MiscUtil::htmlDirectoryPrefix(const std::string &fileName) {
	ZLFile file(fileName);
	std::string shortName = file.name(false);
	std::string path = file.path();
	int index = -1;
	if ((path.length() > shortName.length()) &&
			(path[path.length() - shortName.length() - 1] == ':')) {
		index = shortName.rfind('/');
	}
	return path.substr(0, path.length() - shortName.length() + index + 1);
}

std::string MiscUtil::htmlFileName(const std::string &fileName) {
	ZLFile file(fileName);
	std::string shortName = file.name(false);
	std::string path = file.path();
	int index = -1;
	if ((path.length() > shortName.length()) &&
			(path[path.length() - shortName.length() - 1] == ':')) {
		index = shortName.rfind('/');
	}
	return path.substr(path.length() - shortName.length() + index + 1);
}

std::string MiscUtil::decodeHtmlURL(const std::string &encoded) {
	char buffer[3];
	buffer[2] = '\0';

	std::string decoded;
	const int len = encoded.length();
	decoded.reserve(len);
	for (int i = 0; i < len; i++) {
		if ((encoded[i] == '%') && (i < len - 2)) {
			buffer[0] = *(encoded.data() + i + 1);
			buffer[1] = *(encoded.data() + i + 2);
			decoded += (char)strtol(buffer, 0, 16);
			i += 2;
		} else {
			decoded += encoded[i];
		}
	}
	return decoded;
}
