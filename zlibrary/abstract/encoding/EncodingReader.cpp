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

#include <stdlib.h>

#include <abstract/ZLUnicodeUtil.h>

#include "EncodingReader.h"
#include "ZLEncodingConverter.h"

EncodingReader::EncodingReader(const std::string &encoding) : myFilePath(ZLEncodingConverter::encodingDescriptionPath() + '/' + encoding) {
}

EncodingReader::~EncodingReader() {
}

static const std::string ENCODING = "encoding";
static const std::string CHAR = "char";

void EncodingReader::startElementHandler(const char *tag, const char **attributes) {
  static const std::string BYTES = "bytes";

	if (ENCODING == tag) {
		myBytesNumber = 1;
		if ((attributes[0] != 0) && (BYTES == attributes[0])) {
			myBytesNumber = atoi(attributes[1]);
		}
	}
}

EncodingIntReader::EncodingIntReader(const std::string &encoding) : EncodingReader(encoding) {
}

EncodingIntReader::~EncodingIntReader() {
}

bool EncodingIntReader::fillTable(int *map) {
	myMap = map;
	for (int i = 0; i < 256; ++i) {
		myMap[i] = i;
	}
	return readDocument(myFilePath);
}

void EncodingIntReader::startElementHandler(const char *tag, const char **attributes) {
	EncodingReader::startElementHandler(tag, attributes);
	if ((CHAR == tag) && (attributes[0] != 0) && (attributes[2] != 0)) {
		char *ptr = 0;
		myMap[strtol(attributes[1], &ptr, 16)] = strtol(attributes[3], &ptr, 16);
	}
}

EncodingCharReader::EncodingCharReader(const std::string &encoding) : EncodingReader(encoding) {
}

EncodingCharReader::~EncodingCharReader() {
}

char **EncodingCharReader::createTable() {
	myMap = 0;
	if (!readDocument(myFilePath) && (myMap != 0)) {
		int length = (myBytesNumber == 1) ? 256 : 32768;
		for (int i = 0; i < length; ++i) {
			if (myMap[i] != 0) {
				delete[] myMap[i];
			}
		}
		delete[] myMap;
		myMap = 0;
	}

	return myMap;
}

void EncodingCharReader::startElementHandler(const char *tag, const char **attributes) {
	EncodingReader::startElementHandler(tag, attributes);

	if (ENCODING == tag) {
		int length = (myBytesNumber == 1) ? 256 : 32768;
		myMap = new char*[length];
		memset(myMap, 0, length * sizeof(char*));
	} else if ((CHAR == tag) && (attributes[0] != 0) && (attributes[2] != 0)) {
		static char *ptr = 0;
		int index = strtol(attributes[1], &ptr, 16);
		if (myBytesNumber == 1) {
			if ((index < 0) || (index >= 256)) {
				return;
			}
		} else {
			index -= 32768;
			if ((index < 0) || (index >= 32768)) {
				return;
			}
		}
		int value = strtol(attributes[3], &ptr, 16);
		int len = ZLUnicodeUtil::ucs2ToUtf8(myBuffer, value);
		myMap[index] = new char[len + 1];
		memcpy(myMap[index], myBuffer, len);
		myMap[index][len] = '\0';
	}
}
