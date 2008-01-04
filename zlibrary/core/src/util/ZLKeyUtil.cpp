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
#include <cctype>

#include <ZLUnicodeUtil.h>
#include <ZLStringUtil.h>
#include <ZLXMLReader.h>
#include <ZLibrary.h>

#include "ZLKeyUtil.h"

bool ZLKeyUtil::ourInitialized = false;
bool ZLKeyUtil::ourUseAutoNames = true;
std::map<int,std::string> ZLKeyUtil::ourNames;
std::map<int,std::string> ZLKeyUtil::ourModifiers;
std::string ZLKeyUtil::ourKeyNamesFileName = "keynames.xml";

void ZLKeyUtil::setKeyNamesFileName(const std::string &fileName) {
	ourKeyNamesFileName = fileName;
}

class KeyNamesReader : public ZLXMLReader {

private:
	void startElementHandler(const char *tag, const char **attributes);
};

void KeyNamesReader::startElementHandler(const char *tag, const char **attributes) {
	static const std::string KEY = "key";
	static const std::string MODIFIER = "modifier";

	const char *disableAutoNames = attributeValue(attributes, "disableAutoNames");
	if ((disableAutoNames != 0) && ((std::string("true") == disableAutoNames))) {
		ZLKeyUtil::ourUseAutoNames = false;
	}
	const char *codeString = attributeValue(attributes, "code");
	const char *name = attributeValue(attributes, "name");
	if ((codeString != 0) && (name != 0)) {
		if (KEY == tag) {
			ZLKeyUtil::ourNames[strtol(codeString, 0, 16)] = name;
		} else if (MODIFIER == tag) {
			ZLKeyUtil::ourModifiers[strtol(codeString, 0, 16)] = name;
		}
	}
}

std::string ZLKeyUtil::keyName(int unicode, int key, int modifiersMask) {
	if (!ourInitialized) {
		KeyNamesReader().readDocument(ZLibrary::ZLibraryDirectory() + ZLibrary::FileNameDelimiter + ourKeyNamesFileName);
		ourInitialized = true;
	}

	std::string name;
	std::map<int,std::string>::const_iterator it = ourNames.find(key);
	if (it != ourNames.end()) {
		name = it->second;
	}

	if (ourUseAutoNames && name.empty()) {
		if (((unicode < 128) && isprint(unicode) && !isspace(unicode)) || ZLUnicodeUtil::isLetter(unicode)) {
			name += '<';
			char buf[5];
			name.append(buf, ZLUnicodeUtil::ucs2ToUtf8(buf, ZLUnicodeUtil::toUpper(unicode)));
			name += '>';
		}
	}

	if (name.empty()) {
		name += '[';
		ZLStringUtil::appendNumber(name, key);
		name += ']';
	}

	for (std::map<int,std::string>::iterator it = ourModifiers.begin(); it != ourModifiers.end(); ++it) {
		if ((modifiersMask & it->first) == it->first) {
			name = it->second + "+" + name;
		}
	}

	return name;
}
