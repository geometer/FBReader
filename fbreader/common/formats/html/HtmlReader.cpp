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

#include <cctype>

#include <abstract/ZLInputStream.h>
#include <abstract/ZLStringInputStream.h>

#include <abstract/ZLXMLReader.h>
#include <abstract/ZLFSManager.h>
#include <abstract/ZLFSDir.h>
#include <abstract/ZLStringUtil.h>

#include "../docbook/DocBookReader.h"
#include "HtmlReader.h"

static const ZLXMLReader::Tag TAGS[] = {
	{0, 0}
};

const ZLXMLReader::Tag *HtmlTextConverter::tags() const {
	return TAGS;
}

void HtmlTextConverter::convertBuffer(std::vector<std::string> &buffer) {
	myBuffer = &buffer;
	std::string str = "<t>";
	ZLStringUtil::append(str, buffer);
	str += "</t>";
	buffer.clear();
	ZLStringInputStream stream(str);
	readDocument(stream);
}

void HtmlTextConverter::convertString(std::string &str) {
	std::vector<std::string> buffer;
	myBuffer = &buffer;
	str = "<t>" + str + "</t>";
	ZLStringInputStream stream(str);
	readDocument(stream);
	str.erase();
	ZLStringUtil::append(str, buffer);
}

void HtmlTextConverter::startElementHandler(int, const char **) {
}

void HtmlTextConverter::endElementHandler(int) {
}

void HtmlTextConverter::characterDataHandler(const char *text, int len) {
	if (myBuffer != 0) {
		myBuffer->push_back(std::string());
		myBuffer->back().append(text, len);
	}
}

static std::vector<std::string> EXTERNAL_DTDs;

const std::vector<std::string> &HtmlTextConverter::externalDTDs() const {
	if (EXTERNAL_DTDs.empty()) {
		std::vector<std::string> files;
		ZLFSDir *dtdPath = ZLFSManager::instance().createDirectory(DocBookReader::DTDDirectory);
		dtdPath->collectFiles(files, false);
		for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); it++) {
			if (ZLStringUtil::stringEndsWith(*it, ".ent")) {
				EXTERNAL_DTDs.push_back(dtdPath->name() + "/" + *it);
			}
		}
		delete dtdPath;
	}

	return EXTERNAL_DTDs;
}

HtmlReader::HtmlReader() {
	myConverter = 0;
}

HtmlReader::~HtmlReader() {
	if (myConverter != 0) {
		delete myConverter;
	}
}

static struct {
	const char *tagName;
	HtmlReader::TagCode tagCode;
} HTML_TAGS[] = {
	{ "HEAD", HtmlReader::_HEAD },
	{ "BODY", HtmlReader::_BODY },
	{ "TITLE", HtmlReader::_TITLE },
	{ "H1", HtmlReader::_H1 },
	{ "H2", HtmlReader::_H2 },
	{ "H3", HtmlReader::_H3 },
	{ "H4", HtmlReader::_H4 },
	{ "H5", HtmlReader::_H5 },
	{ "H6", HtmlReader::_H6 },
	// 9. text
	{ "EM", HtmlReader::_EM, },
	{ "STRONG", HtmlReader::_STRONG, },
	{ "DFN", HtmlReader::_DFN, },
	{ "CODE", HtmlReader::_CODE, },
	{ "SAMP", HtmlReader::_SAMP, },
	{ "KBD", HtmlReader::_KBD, },
	{ "VAR", HtmlReader::_VAR, },
	{ "CITE", HtmlReader::_CITE, },
	{ "ABBR", HtmlReader::_ABBR, },
	{ "ACRONYM", HtmlReader::_ACRONYM, },
	{ "BLOCKQUOTE", HtmlReader::_BLOCKQUOTE, },
	{ "Q", HtmlReader::_Q, },
	{ "SUB", HtmlReader::_SUB, },
	{ "SUP", HtmlReader::_SUP, },
	{ "P", HtmlReader::_P, },
	{ "BR", HtmlReader::_BR, },
	{ "PRE", HtmlReader::_PRE, },
	{ "INS", HtmlReader::_INS, },
	{ "DEL", HtmlReader::_DEL, },
	// 10. lists
	{ "UL", HtmlReader::_UL },
	{ "OL", HtmlReader::_OL },
	{ "LI", HtmlReader::_LI },
	{ "DL", HtmlReader::_DL },
	{ "DT", HtmlReader::_DT },
	{ "DD", HtmlReader::_DD },
	{ "MENU", HtmlReader::_UL },
	{ "DIR", HtmlReader::_UL },
	//
	{ "TT", HtmlReader::_TT },
	{ "B", HtmlReader::_B },
	{ "I", HtmlReader::_I },
	{ "STYLE", HtmlReader::_STYLE },
	{ "A", HtmlReader::_A },
	{ "SCRIPT", HtmlReader::_SCRIPT },
	{ 0, HtmlReader::_UNKNOWN }
};

HtmlReader::HtmlTag HtmlReader::tag(std::string &name) {
	if (name.length() == 0) {
		return HtmlTag(_UNKNOWN, true);
	}

	bool start = name[0] != '/';
	if (!start) {
		name = name.substr(1);
	}

	if (name.length() > 10) {
		return HtmlTag(_UNKNOWN, start);
	}

	for (unsigned int i = 0; i < name.length(); i++) {
		name[i] = toupper(name[i]);
	}

	for (unsigned int i = 0; ; i++) {
		if ((HTML_TAGS[i].tagName == 0) || HTML_TAGS[i].tagName == name) {
			return HtmlTag(HTML_TAGS[i].tagCode, start);
		}
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

void HtmlReader::readDocument(ZLInputStream &stream, const std::string &encoding) {
	if (!stream.open()) {
		return;
	}

	if (myConverter != 0) {
		delete myConverter;
	}
	myConverter = new HtmlTextConverter(encoding.c_str());
	std::string startString = "<t>";
	ZLStringInputStream startStream(startString);
	myConverter->readDocument(startStream);

	startDocumentHandler();

	ParseState state = PS_TEXT;
	std::string currentString;
	int quotationCounter = 0;
	HtmlTag currentTag(_UNKNOWN, false);
	char endOfComment[2] = "\0";
	
	const size_t BUFSIZE = 2048;
	char *buffer = new char[BUFSIZE];
	size_t length;
	do {
		length = stream.read(buffer, BUFSIZE);
		char *start = buffer;
		char *endOfBuffer = buffer + length;
		for (char *ptr = buffer; ptr < endOfBuffer; ptr++) {
			switch (state) {
				case PS_TEXT:
					if (*ptr == '<') {
						if (!characterDataHandler(start, ptr - start)) {
							goto endOfProcessing;
						}
						start = ptr + 1;
						state = PS_TAGSTART;
					}
					break;
				case PS_TAGSTART:
					state = (*ptr == '!') ? PS_COMMENT : PS_TAGNAME;
					break;
				case PS_COMMENT:
					if ((endOfComment[0] == '\0') && (*ptr != '-')) {
						state = PS_TAGNAME;
					} else if ((endOfComment[0] == '-') && (endOfComment[1] == '-') && (*ptr == '>')) {
						start = ptr + 1;
						state = PS_TEXT;
						endOfComment[0] = '\0';
						endOfComment[1] = '\0';
					} else {
						endOfComment[0] = endOfComment[1];
						endOfComment[1] = *ptr;
					}
					break;
				case PS_TAGNAME:
					if ((*ptr == '>') || isspace(*ptr)) {
						currentString.append(start, ptr - start);
						start = ptr + 1;
						currentTag = tag(currentString);
						currentString.erase();
						if (currentTag.Code == _UNKNOWN) {
							state = (*ptr == '>') ? PS_TEXT : PS_SKIPTAG;
						} else {
							if (*ptr == '>') {
								if (!tagHandler(currentTag)) {
									goto endOfProcessing;
								}
								state = PS_TEXT;
							} else {
								state = PS_ATTRIBUTENAME;
							}
						}
					}
					break;
				case PS_ATTRIBUTENAME:
					if ((*ptr == '>') || (*ptr == '=') || isspace(*ptr)) {
						if (ptr != start) {
							currentString.append(start, ptr - start);
							for (unsigned int i = 0; i < currentString.length(); i++) {
								currentString[i] = toupper(currentString[i]);
							}
							currentTag.addAttribute(currentString);
							currentString.erase();
						}
						start = ptr + 1;
						if (*ptr == '>') {
							if (!tagHandler(currentTag)) {
								goto endOfProcessing;
							}
							state = PS_TEXT;
						} else {
							state = (*ptr == '=') ? PS_ATTRIBUTEVALUE : PS_ATTRIBUTENAME;
						}
					}
					break;
				case PS_ATTRIBUTEVALUE:
					if (*ptr == '"') {
						if ((ptr == start) || (quotationCounter > 0)) {
							quotationCounter++;
						}
					} else if ((quotationCounter != 1) && ((*ptr == '>') || isspace(*ptr))) {
						if (ptr != start) {
							currentString.append(start, ptr - start);
							if (currentString[0] == '"') {
								currentString = currentString.substr(1, currentString.length() - 2);
							}
							currentTag.setLastAttributeValue(currentString);
							currentString.erase();
							quotationCounter = 0;
						}
						start = ptr + 1;
						if (*ptr == '>') {
							if (!tagHandler(currentTag)) {
								goto endOfProcessing;
							}
							state = PS_TEXT;
						} else {
							state = PS_ATTRIBUTENAME;
						}
					}
					break;
				case PS_SKIPTAG:
					if (*ptr == '>') {
						start = ptr + 1;
						state = PS_TEXT;
					}
					break;
			}
		}
		if (start != endOfBuffer) {
			switch (state) {
				case PS_TEXT:
					if (!characterDataHandler(start, endOfBuffer - start)) {
						goto endOfProcessing;
					}
					break;
				case PS_TAGNAME:
				case PS_ATTRIBUTENAME:
				case PS_ATTRIBUTEVALUE:
					currentString.append(start, endOfBuffer - start);
					break;
				case PS_TAGSTART:
				case PS_SKIPTAG:
				case PS_COMMENT:
					break;
			}
		}
  } while (length == BUFSIZE);
endOfProcessing:
	delete[] buffer;

	endDocumentHandler();

	stream.close();
}
