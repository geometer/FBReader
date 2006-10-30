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

#include <algorithm>

#include <ZLFile.h>
#include <ZLInputStream.h>

#include "../encoding/EncodingReader.h"

#include "ZLXMLReader.h"

#include "expat/ZLXMLReaderInternal.h"

void ZLXMLReader::startElementHandler(const char*, const char**) {
}

void ZLXMLReader::endElementHandler(const char*) {
}

void ZLXMLReader::characterDataHandler(const char*, int) {
}

ZLXMLReader::ZLXMLReader(const char *encoding) {
	myInternalReader = new ZLXMLReaderInternal(*this, encoding);
	myParserBuffer = new char[bufferSize()];
}

ZLXMLReader::~ZLXMLReader() {
	delete[] myParserBuffer;
	delete myInternalReader;
}

bool ZLXMLReader::readDocument(const std::string &fileName) {
	return readDocument(ZLFile(fileName).inputStream());
}

bool ZLXMLReader::readDocument(shared_ptr<ZLInputStream> stream) {
	if (stream.isNull() || !stream->open()) {
		return false;
	}

	myInternalReader->init();

	myInterrupted = false;

	size_t length;
	do {
		length = stream->read(myParserBuffer, bufferSize());
		if (!myInternalReader->parseBuffer(myParserBuffer, length)) {
			break;
		}
	} while ((length == bufferSize()) && !myInterrupted);

	stream->close();

	return true;
}

static const std::vector<std::string> EMPTY_VECTOR;

const std::vector<std::string> &ZLXMLReader::externalDTDs() const {
	return EMPTY_VECTOR;
}

const char *ZLXMLReader::attributeValue(const char **xmlattributes, const char *name) {
	while (*xmlattributes != 0) {
		bool useNext = strcmp(*xmlattributes, name) == 0;
		++xmlattributes;
		if (*xmlattributes == 0) {
			return 0;
		}
		if (useNext) {
			return *xmlattributes;
		}
		++xmlattributes;
	}
	return 0;
}

const char *ZLXMLReader::attributeValue(const char **xmlattributes, const std::string &name) {
	while (*xmlattributes != 0) {
		bool useNext = name == *xmlattributes;
		++xmlattributes;
		if (*xmlattributes == 0) {
			return 0;
		}
		if (useNext) {
			return *xmlattributes;
		}
		++xmlattributes;
	}
	return 0;
}
