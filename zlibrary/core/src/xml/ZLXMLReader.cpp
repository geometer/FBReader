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

#include <string.h>

#include <algorithm>

#include <ZLFile.h>
#include <ZLInputStream.h>
#include <ZLUnicodeUtil.h>
#include <ZLEncodingConverter.h>

#include "ZLXMLReader.h"

#include "expat/ZLXMLReaderInternal.h"

static const size_t BUFFER_SIZE = 2048;

void ZLXMLReader::startElementHandler(const char*, const char**) {
}

void ZLXMLReader::endElementHandler(const char*) {
}

void ZLXMLReader::characterDataHandler(const char*, int) {
}

void ZLXMLReader::namespaceListChangedHandler() {
}

const std::map<std::string,std::string> &ZLXMLReader::namespaces() const {
	return *myNamespaces.back();
}

ZLXMLReader::ZLXMLReader(const char *encoding) {
	myInternalReader = new ZLXMLReaderInternal(*this, encoding);
	myParserBuffer = new char[BUFFER_SIZE];
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

	bool useWindows1252 = false;
	if (ZLEncodingCollection::useWindows1252Hack()) {
		stream->read(myParserBuffer, 256);
		std::string stringBuffer(myParserBuffer, 256);
		stream->seek(0, true);
		int index = stringBuffer.find('>');
		if (index > 0) {
			stringBuffer = ZLUnicodeUtil::toLower(stringBuffer.substr(0, index));
			int index = stringBuffer.find("\"iso-8859-1\"");
			if (index > 0) {
				useWindows1252 = true;
			}
		}
	}
	myInternalReader->init(useWindows1252 ? "windows-1252" : 0);

	myInterrupted = false;

	myNamespaces.push_back(new std::map<std::string, std::string>());

	size_t length;
	do {
		length = stream->read(myParserBuffer, BUFFER_SIZE);
		if (!myInternalReader->parseBuffer(myParserBuffer, length)) {
			break;
		}
	} while ((length == BUFFER_SIZE) && !myInterrupted);

	stream->close();

	myNamespaces.clear();

	return true;
}

bool ZLXMLReader::processNamespaces() const {
	return false;
}

const std::vector<std::string> &ZLXMLReader::externalDTDs() const {
	static const std::vector<std::string> EMPTY_VECTOR;
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
