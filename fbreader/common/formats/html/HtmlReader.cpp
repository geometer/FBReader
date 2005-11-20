/*
 * FBReader -- electronic book reader
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

#include <cctype>

#include <abstract/ZLInputStream.h>
#include <abstract/ZLXMLReader.h>
#include <abstract/ZLFSManager.h>
#include <abstract/ZLStringUtil.h>
#include <abstract/ZLUnicodeUtil.h>

#include "HtmlReader.h"
#include "HtmlEntityExtension.h"

HtmlReader::HtmlReader(const std::string &encoding) {
	myConverter = ZLEncodingConverter::createConverter(encoding);
}

HtmlReader::~HtmlReader() {
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
	{ "DIV", HtmlReader::_DIV },
	{ "IMG", HtmlReader::_IMAGE },
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
	{ "SELECT", HtmlReader::_SELECT },
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
	PS_SPECIAL,
};

enum SpecialType {
	ST_UNKNOWN,
	ST_NUM,
	ST_NAME,
	ST_DEC,
	ST_HEX
};

static bool allowSymbol(SpecialType type, char ch) {
	return
		((type == ST_NAME) && isalpha(ch)) ||
		((type == ST_DEC) && isdigit(ch)) ||
		((type == ST_HEX) && isxdigit(ch));
}

static int specialSymbolNumber(SpecialType type, const std::string &txt) {
	char *end = 0;
	switch (type) {
		case ST_NAME:
			return HtmlEntityExtension::symbolNumber(txt);
		case ST_DEC:
			return strtol(txt.c_str() + 1, &end, 10);
		case ST_HEX:
			return strtol(txt.c_str() + 2, &end, 16);
		default:
			return 0;
	}
}

void HtmlReader::readDocument(ZLInputStream &stream) {
	if (!stream.open()) {
		return;
	}

	startDocumentHandler();

	ParseState state = PS_TEXT;
	SpecialType state_special = ST_UNKNOWN;
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
						if (!characterDataHandler(start, ptr - start, true)) {
							goto endOfProcessing;
						}
						start = ptr + 1;
						state = PS_TAGSTART;
					}
					if (*ptr == '&') {
						if (!characterDataHandler(start, ptr - start, true)) {
							goto endOfProcessing;
						}
						start = ptr + 1;
						state = PS_SPECIAL;
						state_special = ST_UNKNOWN;
					}
					break;
				case PS_SPECIAL:
					if (state_special == ST_UNKNOWN) {
						if (*ptr == '#') {
							state_special = ST_NUM;
						} else if (isalpha(*ptr)) {
							state_special = ST_NAME;
						} else {
							start = ptr;
							state = PS_TEXT;
						}
					} else if (state_special == ST_NUM) {
						if (*ptr == 'x') {
							state_special = ST_HEX;
						} else if (isdigit(*ptr)) {
							state_special = ST_DEC;
						} else {
							start = ptr;
							state = PS_TEXT;
						}
					} else {
						if (*ptr == ';') {
							currentString.append(start, ptr - start);
							int number = specialSymbolNumber(state_special, currentString);
							if (number != 0) {
								char buffer[4];
								int len = ZLUnicodeUtil::ucs2ToUtf8(buffer, number);
								characterDataHandler(buffer, len, false);
							} else {
								currentString = "&" + currentString + ";";
								characterDataHandler(currentString.c_str(), currentString.length(), false);
							}
							currentString.erase();
							start = ptr + 1;
							state = PS_TEXT;
						} else if (!allowSymbol(state_special, *ptr)) {
							start = ptr;
							state = PS_TEXT;
						}
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
					if (!characterDataHandler(start, endOfBuffer - start, true)) {
						goto endOfProcessing;
					}
					break;
				case PS_TAGNAME:
				case PS_ATTRIBUTENAME:
				case PS_ATTRIBUTEVALUE:
				case PS_SPECIAL:
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
