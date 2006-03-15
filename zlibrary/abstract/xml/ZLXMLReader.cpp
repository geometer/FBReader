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

#include "../filesystem/ZLFSManager.h"
#include "../filesystem/ZLFSDir.h"
#include "../filesystem/ZLInputStream.h"

#include "../encoding/EncodingReader.h"

#include "ZLXMLReader.h"

#ifdef USE_OWN_XML_PARSER
#include "own/ZLXMLReaderInternal.h"
#else // USE_OWN_XML_PARSER
#include "expat/ZLXMLReaderInternal.h"
#endif // USE_OWN_XML_PARSER

void ZLXMLReader::startElementHandler(const char *t, const char **attributes) {
	startElementHandler(tag(t), attributes);
}

void ZLXMLReader::endElementHandler(const char *t) {
	endElementHandler(tag(t));
}

int ZLXMLReader::tag(const char *name) {
	const Tag *_tags = tags();
	for (int i = 0; ; i++) {
		if ((_tags[i].tagName == 0) || (strcmp(name, _tags[i].tagName) == 0)) {
			return _tags[i].tagCode;
		}
	}
}

ZLXMLReaderBase::ZLXMLReaderBase(const char *encoding) {
	myInternalReader = new ZLXMLReaderInternal(*this, encoding);
	myParserBuffer = new char[bufferSize()];
}

ZLXMLReaderBase::~ZLXMLReaderBase() {
	delete[] myParserBuffer;
	delete myInternalReader;
}

bool ZLXMLReaderBase::readDocument(shared_ptr<ZLInputStream> stream) {
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

static std::vector<std::string> EMPTY_VECTOR;

const std::vector<std::string> &ZLXMLReaderBase::externalDTDs() const {
	return EMPTY_VECTOR;
}

const char *ZLXMLReaderBase::attributeValue(const char **xmlattributes, const char *name) {
	while (*xmlattributes != 0) {
		bool useNext = strcmp(*xmlattributes, name) == 0;
		xmlattributes++;
		if (*xmlattributes == 0) {
			return 0;
		}
		if (useNext) {
			return *xmlattributes;
		}
		xmlattributes++;
	}
	return 0;
}
