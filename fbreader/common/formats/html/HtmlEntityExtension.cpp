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

#include <cctype>

#include <abstract/ZLInputStream.h>
#include <abstract/ZLFSManager.h>

#include "HtmlEntityExtension.h"

std::map<std::string,int> HtmlEntityExtension::ourSymbolicEntityCollection;

HtmlEntityExtension::HtmlEntityExtension() {
	if (ourSymbolicEntityCollection.empty()) {
		ZLInputStream *stream = ZLFSManager::instance().createInputStream(CollectionFile);
		if (stream != 0) {
			CollectionReader().readDocument(*stream);
			delete stream;
		}
	}
}

bool HtmlEntityExtension::parseCharacter(char ch) {
	myBuffer += ch;

	if (myParseType == UNDEFINED) {
		switch (myBuffer.length()) {
			case 2:
				if (ch != '#') {
					myParseType = AS_SYMBOLIC;
				}
				break;
			case 3:
				if (ch == 'x') {
					myParseType = AS_HEXADECIMAL;
				} else if (isdigit(ch)) {
					myParseType = AS_DECIMAL;
				} else {
					return true;
				}
				break;
		}
		return false;
	}

	if (ch == ';') {
		int num = 0;
		switch (myParseType) {
			case AS_SYMBOLIC:
			{
				std::string name = myBuffer.substr(1, myBuffer.length() - 2);
				std::map<std::string,int>::const_iterator it = ourSymbolicEntityCollection.find(name);
				if (it != ourSymbolicEntityCollection.end()) {
					num = it->second;
				}
				break;
			}
			case AS_DECIMAL:
				for (const char *ptr = myBuffer.data() + 2; *ptr != ';'; ptr++) {
					num *= 10;
					num += *ptr - '0';
				}
				break;
			case AS_HEXADECIMAL:
				for (const char *ptr = myBuffer.data() + 3; *ptr != ';'; ptr++) {
					num *= 16;
					if (isdigit(*ptr)) {
						num += *ptr - '0';
					} else {
						num += tolower(*ptr) - 'a' + 10;
					}
				}
				break;
			default:
				break;
		}
		if (num > 0) {
			myBuffer.erase();
      if (num < 0x80) {
				myBuffer += (char)num;
			} else if (num < 0x800) {
				myBuffer += (char)(0xC0 | num >> 6);
				myBuffer += (char)(0x80 | num & 0x3F);
			} else {
				myBuffer += (char)(0xE0 | num >> 12);
				myBuffer += (char)(0x80 | num >> 6 & 0x3F);
				myBuffer += (char)(0x80 | num & 0x3F);
			}
		}
		return true;
	}

	switch (myParseType) {
		case AS_SYMBOLIC:
			if (!isalpha(ch)) {
				return true;
			}
			break;
		case AS_DECIMAL:
			if (!isdigit(ch)) {
				return true;
			}
			break;
		case AS_HEXADECIMAL:
			if (!isxdigit(ch)) {
				return true;
			}
			break;
		default:
			break;
	}

	return myBuffer.length() >= 10;
}

static ZLXMLReader::Tag TAGS[] = {
	{ "entity", 1 },
	{ 0, 0 }
};

const ZLXMLReader::Tag *HtmlEntityExtension::CollectionReader::tags() const {
	return TAGS;
}

void HtmlEntityExtension::CollectionReader::startElementHandler(int tag, const char **attributes) {
	if (tag == 1) {
		for (int i = 0; i < 4; i++) {
			if (attributes[i] == 0) {
				return;
			}
		}
		static const std::string _name = "name";
		static const std::string _number = "number";
		if ((_name == attributes[0]) && (_number == attributes[2])) {
			ourSymbolicEntityCollection[attributes[1]] = atoi(attributes[3]);
		}
	}
}
