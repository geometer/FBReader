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

#include "../filesystem/ZLFSManager.h"
#include "../filesystem/ZLFSDir.h"
#include "../filesystem/ZLInputStream.h"

#include "ZLXMLReader.h"
#include "EncodingReader.h"

#ifdef USE_OWN_XML_PARSER
#include "own/ZLXMLReaderInternal.h"
#else // USE_OWN_XML_PARSER
#include "expat/ZLXMLReaderInternal.h"
#endif // USE_OWN_XML_PARSER

std::string ZLXMLReader::ourEncodingDescriptionPath;
std::vector<std::string> ZLXMLReader::ourKnownEncodings;

void ZLXMLReader::setEncodingDescriptionPath(const std::string &path) {
	ourEncodingDescriptionPath = path;
	ourKnownEncodings.clear();
	ZLFSDir *dir = ZLFSManager::instance().createDirectory(ourEncodingDescriptionPath);
	dir->collectFiles(ourKnownEncodings, false);
	ourKnownEncodings.push_back("US-ASCII");
	ourKnownEncodings.push_back("UTF-8");
	delete dir;
}

int ZLXMLReader::tag(const char *name) {
	const Tag *_tags = tags();
	for (int i = 0; ; i++) {
		if ((_tags[i].tagName == 0) || (strcmp(name, _tags[i].tagName) == 0)) {
			return _tags[i].tagCode;
		}
	}
}

const size_t BUFSIZE = 2048;

ZLXMLReader::ZLXMLReader(const char *encoding) {
	myInternalReader = new ZLXMLReaderInternal(*this, encoding);
	myParserBuffer = new char[BUFSIZE];
}

ZLXMLReader::~ZLXMLReader() {
	delete[] myParserBuffer;
	delete myInternalReader;
}

bool ZLXMLReader::readDocument(ZLInputStream &stream) {
	if (!stream.open()) {
		return false;
	}

	myInternalReader->init();

	myDoBreak = false;

	size_t length;
	do {
		length = stream.read(myParserBuffer, BUFSIZE);
		if (!myInternalReader->parseBuffer(myParserBuffer, length)) {
			break;
    }
  } while ((length == BUFSIZE) && !myDoBreak);

	stream.close();

	return true;
}

static std::vector<std::string> EMPTY_VECTOR;

const std::vector<std::string> &ZLXMLReader::externalDTDs() const {
	return EMPTY_VECTOR;
}
