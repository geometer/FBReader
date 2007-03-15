/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <ZLApplication.h>
#include <ZLFile.h>

#include "MiscUtil.h"

static bool hasPrefix(const std::string &text, const std::string &prefix) {
	return (text.length() >= prefix.length()) && (text.substr(0, prefix.length()) == prefix);
}

bool MiscUtil::isReference(const std::string &text) {
	return
		hasPrefix(text, "http://") ||
		hasPrefix(text, "https://") ||
		hasPrefix(text, "mailto:") ||
		hasPrefix(text, "ftp://");
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
