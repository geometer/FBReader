/*
 * FBReader -- electronic book reader
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

#include <abstract/ZLInputStream.h>
#include <abstract/ZLStringInputStream.h>

#include <abstract/ZLXMLReader.h>
#include <abstract/ZLFSDir.h>
#include <abstract/ZLStringUtil.h>

#include "../docbook/DocBookReader.h"
#include "TxtReader.h"

static const ZLXMLReader::Tag TAGS[] = {
	{0, 0}
};

const ZLXMLReader::Tag *TxtTextConverter::tags() const {
	return TAGS;
}

void TxtTextConverter::convertBuffer(std::vector<std::string> &buffer) {
	/*
	myBuffer = &buffer;
	std::string str = "<t>";
	ZLStringUtil::append(str, buffer);
	str += "</t>";
	buffer.clear();
	ZLStringInputStream stream(str);
	readDocument(stream);
	*/
}

void TxtTextConverter::convertString(std::string &str) {
	std::vector<std::string> buffer;
	myBuffer = &buffer;
	str = "<t>" + str + "</t>";
	ZLStringInputStream stream(str);
	readDocument(stream);
	str.erase();
	ZLStringUtil::append(str, buffer);
}

void TxtTextConverter::startElementHandler(int, const char **) {
}

void TxtTextConverter::endElementHandler(int) {
}

void TxtTextConverter::characterDataHandler(const char *text, int len) {
	if (myBuffer != 0) {
		myBuffer->push_back(std::string());
		myBuffer->back().append(text, len);
	}
}

TxtReader::TxtReader() {
	myConverter = 0;
}

TxtReader::~TxtReader() {
	if (myConverter != 0) {
		delete myConverter;
	}
}

enum ParseState {
	PS_TEXT,
	PS_TAGSTART,
	PS_TAGNAME,
	PS_ATTRIBUTENAME,
	PS_ATTRIBUTEVALUE,
	PS_SKIPTAG,
	PS_COMMENT,
};

void TxtReader::readDocument(ZLInputStream &stream, const std::string &encoding) {
	if (!stream.open()) {
		return;
	}

	if (myConverter != 0) {
		delete myConverter;
	}
	myConverter = new TxtTextConverter(encoding.c_str());
	std::string startString = "<t>";
	ZLStringInputStream startStream(startString);
	myConverter->readDocument(startStream);

	startDocumentHandler();

	const size_t BUFSIZE = 2048;
	char *buffer = new char[BUFSIZE];
	size_t length;
	do {
		length = stream.read(buffer, BUFSIZE);
		char *ptr = buffer;
		for (size_t i = 0; i < length; i++) {
			if (buffer[i] == '\n') {
				if (ptr != buffer + i) {
					characterDataHandler(ptr, buffer - ptr + i);
				}
				ptr = buffer + i + 1;
				newLineHandler();
			}
		}
		if (ptr != buffer + length) {
			characterDataHandler(ptr, buffer - ptr + length);
		}
  } while (length == BUFSIZE);
	delete[] buffer;

	endDocumentHandler();

	stream.close();
}
