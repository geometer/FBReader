/*
 * Copyright (C) 2004, 2005 Nikolay Pultsin <geometer@mawhrin.net>
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

#include "../filesystem/ZLFSManager.h"
#include "../filesystem/ZLInputStream.h"

#include "EncodingReader.h"

EncodingReader::EncodingReader(const std::string &encoding) : myEncoding(encoding) {
}

EncodingReader::~EncodingReader() {
}

void EncodingReader::endElementHandler(int) {
}

void EncodingReader::characterDataHandler(const char *, int) {
}

bool EncodingReader::fillTable(int *map) {
	myMap = map;
	for (int i = 0; i < 256; i++) {
		myMap[i] = i;
	}
	return readDocument(ZLFile(myEncoding).inputStream());
}

char **EncodingReader::createCharTable() {
	int *intMap = new int[256];
	bool code = fillTable(intMap);
	if (code) {
		char **encodingMap = new char*[256];
		memset(encodingMap, 0, 256);
		char buffer[3];
		for (int i = 0; i < 256; i++) {
			int len = ZLUnicodeUtil::ucs2ToUtf8(buffer, intMap[i]);
			encodingMap[i] = new char[len + 1];
			memcpy(encodingMap[i], buffer, len);
			encodingMap[i][len] = '\0';
		}
		delete[] intMap;
		return encodingMap;
	} else {
		delete[] intMap;
		return 0;
	}
}

ZLXMLReader::Tag TAGS[] = {
	{ "encoding", 0 },
	{ "char", 1 },
	{ 0, 2 }
};

const ZLXMLReader::Tag *EncodingReader::tags() const {
	return TAGS;
}

void EncodingReader::startElementHandler(int tag, const char **attributes) {
	static const std::string BYTES = "bytes";
	if (tag == 0) {
		myBytesNumber = 1;
		if ((attributes[0] != 0) && (BYTES == attributes[0])) {
			myBytesNumber = atoi(attributes[1]);
		}
		if (myBytesNumber == 2) {
		} else {
		}
	} else if ((tag == 1) && (attributes[0] != 0) && (attributes[2] != 0)) {
		char *ptr = 0;
		myMap[strtol(attributes[1], &ptr, 16)] = strtol(attributes[3], &ptr, 16);
	}
}
