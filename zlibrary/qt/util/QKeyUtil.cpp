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

#include <iostream>
#include <cctype>

#include <qevent.h>

#include <abstract/ZLFSManager.h>
#include <abstract/ZLInputStream.h>
#include <abstract/ZLUnicodeUtil.h>
#include <abstract/ZLStringUtil.h>
#include <abstract/ZLXMLReader.h>

#include "QKeyUtil.h"

std::map<int,std::string> QKeyUtil::ourNames;
std::map<int,std::string> QKeyUtil::ourModifiers;

class KeyNamesReader : public ZLXMLReader {

public:
	KeyNamesReader(std::map<int,std::string> &names, std::map<int,std::string> &modifiers);

private:
	void startElementHandler(const char *tag, const char **attributes);

private:
	std::map<int,std::string> &myNames;
	std::map<int,std::string> &myModifiers;
};

std::string QKeyUtil::keyName(QKeyEvent *keyEvent) {
	if (ourNames.empty()) {
		KeyNamesReader(ourNames, ourModifiers).readDocument(ZLFile(FilePath).inputStream());
	}

	QString txt = keyEvent->text();
	int key = keyEvent->key();
	int state = keyEvent->state();

	std::string name;
	if ((txt.length() == 1) && txt[0].isPrint() && !txt[0].isSpace()) {
		name = '<' + ZLUnicodeUtil::toUpper((const char*)txt.utf8()) + '>';
	} else {
		name = keyName(key);
	}
	if (name.empty()) {
		std::string last;
		ZLStringUtil::appendNumber(last, state);
		last += "+";
		ZLStringUtil::appendNumber(last, key);
		return last;
	}

	for (std::map<int,std::string>::iterator it = ourModifiers.begin(); it != ourModifiers.end(); ++it) {
		if ((state & it->first) == it->first) {
			name = it->second + "+" + name;
		}
	}
	return name;
}

KeyNamesReader::KeyNamesReader(std::map<int,std::string> &names, std::map<int,std::string> &modifiers) : myNames(names), myModifiers(modifiers) {
}

void KeyNamesReader::startElementHandler(const char *tag, const char **attributes) {
	static const std::string KEY = "key";
	static const std::string MODIFIER = "modifier";
	if (KEY == tag) {
		const char *codeString = attributeValue(attributes, "code");
		const char *name = attributeValue(attributes, "name");
		if ((codeString != 0) && (name != 0)) {
			myNames[strtol(codeString, 0, 16)] = name;
		}
	} if (MODIFIER == tag) {
		const char *codeString = attributeValue(attributes, "code");
		const char *name = attributeValue(attributes, "name");
		if ((codeString != 0) && (name != 0)) {
			myModifiers[strtol(codeString, 0, 16)] = name;
		}
	}
}

std::string QKeyUtil::keyName(int key) {
	if ((key < 128) && isprint(key) && !isspace(key)) {
		std::string name = "<";
		name += (char)toupper(key);
		name += '>';
		return name;
	}
	std::map<int,std::string>::const_iterator it = ourNames.find(key);
	return (it != ourNames.end()) ? it->second : "";
}
