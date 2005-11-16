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

#include "EncodingConverter.h"
#include "ZLXMLReader.h"
#include "EncodingReader.h"

shared_ptr<EncodingConverter> EncodingConverter::createConverter(const std::string &encoding) {
	if (!encoding.empty()) {
		const std::vector<std::string> &encodingList = ZLXMLReader::knownEncodings();
		std::vector<std::string>::const_iterator it;
		for (it = encodingList.begin(); it != encodingList.end(); it++) {
			if (strcasecmp(encoding.c_str(), it->c_str()) == 0) {
				break;
			}
		}

		if (it != encodingList.end()) {
			char **encodingMap = new char*[256];
			memset(encodingMap, 0, 256);
			EncodingReader er(ZLXMLReader::encodingDescriptionPath() + '/' + *it);
			if (er.fillTable(encodingMap)) {
				return new OneByteEncodingConverter(encodingMap);
			}
			delete[] encodingMap;
		}
	}

	return new DummyEncodingConverter();
}

EncodingConverter::EncodingConverter() : myExtensionNumber(0) {
}

EncodingConverter::~EncodingConverter() {
}

void EncodingConverter::registerExtension(char ch, const shared_ptr<ControlSequenceExtension> extension) {
	if (myExtensions[(unsigned char)ch].isNull()) {
		myExtensionNumber++;
	}
	myExtensions[(unsigned char)ch] = extension;
}

DummyEncodingConverter::DummyEncodingConverter() {
}

DummyEncodingConverter::~DummyEncodingConverter() {
}

void DummyEncodingConverter::convert(std::string &dst, const char *srcStart, const char *srcEnd) {
	if (myExtensionNumber == 0) {
		dst.append(srcStart, srcEnd - srcStart);
	} else {
		dst.reserve(dst.length() + srcEnd - srcStart);
		for (const char *ptr = srcStart; ptr != srcEnd; ptr++) {
			if (myActiveExtension.isNull()) {
				myActiveExtension = myExtensions[(unsigned char)*ptr];
				if (!myActiveExtension.isNull()) {
					myActiveExtension->start();
				}
			}
			if (myActiveExtension.isNull()) {
				dst += *ptr;
			} else {
				if (myActiveExtension->parseCharacter(*ptr)) {
					dst += myActiveExtension->buffer();
					myActiveExtension = 0;
				}
			}
		}
	}
}

OneByteEncodingConverter::OneByteEncodingConverter(char **encodingMap) : myEncodingMap(encodingMap) {
}

OneByteEncodingConverter::~OneByteEncodingConverter() {
	delete[] myEncodingMap;
}

void OneByteEncodingConverter::convert(std::string &dst, const char *srcStart, const char *srcEnd) {
	dst.reserve(dst.length() + 3 * (srcEnd - srcStart));
	for (const char *ptr = srcStart; ptr != srcEnd; ptr++) {
		if (myExtensionNumber > 0) {
			if (myActiveExtension.isNull()) {
				myActiveExtension = myExtensions[(unsigned char)*ptr];
				if (!myActiveExtension.isNull()) {
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

		dst += myEncodingMap[(unsigned char)*ptr];
	}
}
