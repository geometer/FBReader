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

#include <cctype>

#include "EncodingConverter.h"
#include "ZLXMLReader.h"
#include "EncodingReader.h"

EncodingConverter::EncodingConverter(const char *encoding) {
	myEncodingMap = 0;
	if (encoding != 0) {
		setEncoding(encoding);
	}
}

EncodingConverter::~EncodingConverter() {
	if (myEncodingMap != 0) {
		delete[] myEncodingMap;
	}
}

void EncodingConverter::setEncoding(const char *encoding) {
	if (myEncodingMap == 0) {
		myEncodingMap = new int[256];
	}

	const std::vector<std::string> &encodings = ZLXMLReader::knownEncodings();
	for (std::vector<std::string>::const_iterator it = encodings.begin(); it != encodings.end(); it++) {
		if (strcasecmp(encoding, it->c_str()) == 0) {
			EncodingReader er(ZLXMLReader::encodingDescriptionPath() + '/' + *it);
			if (!er.fillTable(myEncodingMap)) {
				if (myExtensions.empty()) {
					delete[] myEncodingMap;
					myEncodingMap = 0;
				} else {
					for (int i = 0; i < 256; i++) {
						myEncodingMap[i] = i;
					}
				}
			}
			break;
		}
	}
}

void EncodingConverter::registerExtension(char ch, const shared_ptr<ControlSequenceExtension> extension) {
	if (myExtensions.empty() && myEncodingMap == 0) {
		myEncodingMap = new int[256];
		for (int i = 0; i < 256; i++) {
			myEncodingMap[i] = i;
		}
	}
	myExtensions[ch] = extension;
}

void EncodingConverter::convert(std::string &dst, const char *srcStart, const char *srcEnd) {
	if (myEncodingMap == 0) {
		dst.append(srcStart, srcEnd - srcStart);
	} else {
		dst.reserve(dst.length() + 3 * (srcEnd - srcStart));
		bool hasExtensions = !myExtensions.empty();
		for (const char *ptr = srcStart; ptr != srcEnd; ptr++) {
			if (hasExtensions) {
				if (myActiveExtension.isNull()) {
					std::map<char,shared_ptr<ControlSequenceExtension> >::const_iterator it = myExtensions.find(*ptr);
					if (it != myExtensions.end()) {
						myActiveExtension = it->second;
						myActiveExtension->start();
					}
				}
				if (!myActiveExtension.isNull()) {
					if (myActiveExtension->parseCharacter(*ptr)) {
						dst += myActiveExtension->buffer();
						myActiveExtension = 0;
					}
					continue;
				}
			}

			int &ucs2code = myEncodingMap[(unsigned char)*ptr];
    	if (ucs2code < 0x80) {
				dst += (char)ucs2code;
			} else if (ucs2code < 0x800) {
				dst += (char)(0xC0 | ucs2code >> 6);
				dst += (char)(0x80 | ucs2code & 0x3F);
			} else {
				dst += (char)(0xE0 | ucs2code >> 12);
				dst += (char)(0x80 | ucs2code >> 6 & 0x3F);
				dst += (char)(0x80 | ucs2code & 0x3F);
			}
		}
	}
}
