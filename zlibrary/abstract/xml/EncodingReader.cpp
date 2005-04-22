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

#include <stdlib.h>

#include "../filesystem/ZLFSManager.h"
#include "../filesystem/ZLInputStream.h"

#include "EncodingReader.h"

EncodingReader::EncodingReader(const std::string &encoding) : myEncoding(encoding) {
}

bool EncodingReader::fillTable(int *map) {
	ZLInputStream *stream = ZLFSManager::instance().createInputStream(myEncoding);
	if (stream == 0) {
		return false;
	}
	myMap = map;
	for (int i = 0; i < 255; i++) {
		myMap[i] = i;
	}
	bool code = readDocument(*stream);
	delete stream;
	return code;
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
	if ((tag == 1) && (attributes[0] != 0) && (attributes[2] != 0)) {
		char *ptr = 0;
		myMap[strtol(attributes[1], &ptr, 16)] = strtol(attributes[3], &ptr, 16);
	}
}
