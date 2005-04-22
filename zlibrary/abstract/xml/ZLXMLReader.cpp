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

static void fCharacterDataHandler(void *userData, const char *text, int len) {
	((ZLXMLReader*)userData)->characterDataHandler(text, len);
}

static void fStartElementHandler(void *userData, const char *name, const char **attributes) {
	((ZLXMLReader*)userData)->startElementHandler(((ZLXMLReader*)userData)->tag(name), attributes);
}

static void fEndElementHandler(void *userData, const char *name) {
	((ZLXMLReader*)userData)->endElementHandler(((ZLXMLReader*)userData)->tag(name));
}

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

static int fUnknownEncodingHandler(void *, const XML_Char *name, XML_Encoding *info) {
	bool code = false;
	const std::vector<std::string> &encodings = ZLXMLReader::knownEncodings();
	for (std::vector<std::string>::const_iterator it = encodings.begin(); it != encodings.end(); it++) {
		if (strcasecmp(name, it->c_str()) == 0) {
			EncodingReader er(ZLXMLReader::encodingDescriptionPath() + '/' + *it);
			code = er.fillTable(info->map);
			break;
		}
	}
	return code ? XML_STATUS_OK : XML_STATUS_ERROR;
}

int ZLXMLReader::tag(const char *name) {
	const Tag *_tags = tags();
	for (int i = 0; ; i++) {
		if ((_tags[i].tagName == 0) || (strcmp(name, _tags[i].tagName) == 0)) {
			return _tags[i].tagCode;
		}
	}
}

static void parseDTD(XML_Parser parser, const std::string &fileName) {
	XML_Parser entityParser = XML_ExternalEntityParserCreate(parser, 0, 0);
	ZLInputStream *entityStream = ZLFSManager::instance().createInputStream(fileName);
	if (entityStream->open()) {
		const size_t BUFSIZE = 2048;
		char buffer[BUFSIZE];
		size_t length;
		do {
			length = entityStream->read(buffer, BUFSIZE);
			if (XML_Parse(entityParser, buffer, length, 0) == XML_STATUS_ERROR) {
				break;
      }
    } while (length == BUFSIZE);
	}
	XML_ParserFree(entityParser);
	delete entityStream;
}

const size_t BUFSIZE = 2048;

ZLXMLReader::ZLXMLReader(const char *encoding) {
	myParser = XML_ParserCreate(encoding);
	myParserBuffer = new char[BUFSIZE];
	myInitialized = false;
}

ZLXMLReader::~ZLXMLReader() {
	delete[] myParserBuffer;

	XML_ParserFree(myParser);
}

bool ZLXMLReader::readDocument(ZLInputStream &stream) {
	if (!stream.open()) {
		return false;
	}

	if (!myInitialized) {
		myInitialized = true;
		XML_UseForeignDTD(myParser, XML_TRUE);

		const std::vector<std::string> &dtds = externalDTDs();
		for (std::vector<std::string>::const_iterator it = dtds.begin(); it != dtds.end(); it++) {
			parseDTD(myParser, *it);
		}

		XML_SetUserData(myParser, this);
		XML_SetStartElementHandler(myParser, fStartElementHandler);
		XML_SetEndElementHandler(myParser, fEndElementHandler);
		XML_SetCharacterDataHandler(myParser, fCharacterDataHandler);
    XML_SetUnknownEncodingHandler(myParser, fUnknownEncodingHandler, NULL);
	}

	myDoBreak = false;

	size_t length;
	do {
		length = stream.read(myParserBuffer, BUFSIZE);
		if (XML_Parse(myParser, myParserBuffer, length, 0) == XML_STATUS_ERROR) {
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
