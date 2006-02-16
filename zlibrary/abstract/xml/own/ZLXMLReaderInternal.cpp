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

#include <cctype>

#include "ZLXMLReaderInternal.h"
#include "../ZLXMLReader.h"
#include "../EncodingReader.h"

ZLXMLReaderInternal::ZLXMLReaderInternal(ZLXMLReader &reader, const char *encoding) : myReader(reader) {
	myEncodingMap = 0;
	myInternalBuffer = 0;
	if (encoding != 0) {
		setEncoding(encoding);
	}
}

ZLXMLReaderInternal::~ZLXMLReaderInternal() {
	if (myEncodingMap != 0) {
		delete[] myEncodingMap;
	}
	if (myInternalBuffer != 0) {
		delete[] myInternalBuffer;
	}
}

void ZLXMLReaderInternal::init() {
	myState = XML_PARSE_DATA;
}

static const char *NULL_ATTRS = { 0 }; 

void ZLXMLReaderInternal::setEncoding(const char *encoding) {
	if (myEncodingMap == 0) {
		myEncodingMap = new int[256];
	}

	const std::vector<std::string> &encodings = ZLXMLReader::knownEncodings();
	for (std::vector<std::string>::const_iterator it = encodings.begin(); it != encodings.end(); it++) {
		if (strcasecmp(encoding, it->c_str()) == 0) {
			EncodingReader er(ZLXMLReader::encodingDescriptionPath() + '/' + *it);
			if (er.fillTable(myEncodingMap)) {
				if (myInternalBuffer == 0) {
					myInternalBuffer = new char[3 * ZLXMLReader::bufferSize()];
				}
			} else {
				delete[] myEncodingMap;
				myEncodingMap = 0;
				if (myInternalBuffer != 0) {
					delete[] myInternalBuffer;
					myInternalBuffer = 0;
				}
			}
			break;
		}
	}
}

void ZLXMLReaderInternal::processData(const char *start, const char *end) {
	if (myEncodingMap == 0) {
		myReader.characterDataHandler(start, end - start);
	} else {
		char *utf8ptr = myInternalBuffer;
		for (const char *ptr = start; ptr != end; ptr++) {
			int &ucs2code = myEncodingMap[(unsigned char)*ptr];
    	if (ucs2code < 0x80) {
				*utf8ptr = (char)ucs2code;
				utf8ptr++;
			} else if (ucs2code < 0x800) {
				*utf8ptr = (char)(0xC0 | ucs2code >> 6);
				utf8ptr++;
				*utf8ptr = (char)(0x80 | ucs2code & 0x3F);
				utf8ptr++;
			} else {
				*utf8ptr = (char)(0xE0 | ucs2code >> 12);
				utf8ptr++;
				*utf8ptr = (char)(0x80 | ucs2code >> 6 & 0x3F);
				utf8ptr++;
				*utf8ptr = (char)(0x80 | ucs2code & 0x3F);
				utf8ptr++;
			}
		}
		myReader.characterDataHandler(myInternalBuffer, utf8ptr - myInternalBuffer);
	}
}

bool ZLXMLReaderInternal::parseBuffer(const char *buffer, size_t len) {
	const char *start = buffer;
	const char *end = buffer + len;
	for (const char *ptr = start; ptr != end; ptr++) {
		switch (myState) {
			case XML_PARSE_DATA:
				switch (*ptr) {
					case '<':
						if (ptr != start) {
							processData(start, ptr);
						}
						start = ptr + 1;
						myState = XML_PARSE_TAG;
						break;
					case '>':
						return false;
					default:
						break;
				}
				break;
			case XML_PARSE_TAG:
				switch (*ptr) {
					case '<':
						return false;
					case '>':
						myTagContents.append(start, ptr - start);
						if (myTagContents.length() > 0) {
							if (myTagContents[0] != '/') {
								size_t tagLen = myTagContents.length();
								size_t nameLen;
								for (nameLen = 0; nameLen < tagLen; nameLen++) {
									if (isspace(myTagContents[nameLen]) || myTagContents[nameLen] == '/') {
										break;
									}
								}
								const char **attributes = &NULL_ATTRS;
								char *attributes_string = 0;
								std::vector<char*> attributes_vector;
								if (nameLen + 1 < tagLen) {
									attributes_string = new char[tagLen - nameLen - 1];
									const char *end = attributes_string + tagLen - nameLen - 1;
									strncpy(attributes_string, myTagContents.data() + nameLen + 1, tagLen - nameLen - 1);
									bool read_text = false;
									for (char *ptr = attributes_string; ptr != end; ptr++) {
										if (isspace(*ptr) || (*ptr == '"') || (*ptr == '=')) {
											*ptr = '\0';
											read_text = false;
										} else if (!read_text) {
											attributes_vector.push_back(ptr);
											read_text = true;
										}
									}
									if (!attributes_vector.empty()) {
										attributes_vector.push_back(0);
										attributes = (const char**)&attributes_vector[0];
									}
								}
								std::string tagName = myTagContents.substr(0, nameLen);
								if (tagName == "?xml") {
									std::string encoding = "encoding";
									int count = 0;
									bool flag = false;
									for (const char **ptr = attributes; *ptr != 0; ptr++, count++) {
										if (flag) {
											setEncoding(*ptr);
											break;
										}
										flag = (count % 2 == 0) && (encoding == *ptr);
									}
								} else {
									myReader.startElementHandler(myReader.tag(tagName.c_str()), attributes);
									if (attributes_string != 0) {
										delete[] attributes_string;
									}
                
									if (myTagContents[tagLen - 1] == '/') {
										myReader.endElementHandler(myReader.tag(tagName.c_str()));
									}
								}
							} else {
								myReader.endElementHandler(myReader.tag(myTagContents.substr(1).c_str()));
							}
						}
						myTagContents.erase();
						start = ptr + 1;
						myState = XML_PARSE_DATA;
						break;
					default:
						break;
				}
		}
	}
	if (end != start) {
		switch (myState) {
			case XML_PARSE_DATA:
				processData(start, end);
				break;
			case XML_PARSE_TAG:
				myTagContents.append(start, end - start);
				break;
		}
	}
	return true;
}
