/*
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
		myEncodingMap = new char*[256];
	} else {
		for (int i = 0; i < 256; i++) {
			delete[] myEncodingMap[i];
		}
	}
	memset(myEncodingMap, 0, 256);

	const std::vector<std::string> &encodings = ZLXMLReader::knownEncodings();
	for (std::vector<std::string>::const_iterator it = encodings.begin(); it != encodings.end(); it++) {
		if (strcasecmp(encoding, it->c_str()) == 0) {
			EncodingReader er(ZLXMLReader::encodingDescriptionPath() + '/' + *it);
			if (!er.fillTable(myEncodingMap)) {
				delete[] myEncodingMap;
				myEncodingMap = 0;
			}
			break;
		}
	}

	if (!myExtensions.empty()) {
		setDummyEncoding();
	}
}

void EncodingConverter::setDummyEncoding() {
	myEncodingMap = new char*[256];
	for (int i = 0; i < 256; i++) {
		myEncodingMap[i] = new char[2];
		myEncodingMap[i][0] = i;
		myEncodingMap[i][1] = '\0';
	}
}

void EncodingConverter::registerExtension(char ch, const shared_ptr<ControlSequenceExtension> extension) {
	if (myExtensions.empty() && (myEncodingMap == 0)) {
		setDummyEncoding();
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

			dst.append(myEncodingMap[(unsigned char)*ptr]);
		}
	}
}
