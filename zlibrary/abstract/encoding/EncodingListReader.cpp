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

#include <iconv.h>

#include <algorithm>

#include "../filesystem/ZLFSManager.h"
#include "../filesystem/ZLInputStream.h"
#include "../xml/ZLXMLReader.h"

#include "ZLEncodingConverter.h"

std::string ZLEncodingConverter::ourEncodingDescriptionPath;
std::vector<std::string> ZLEncodingConverter::ourKnownEncodings;
std::map<int,std::string> ZLEncodingConverter::ourEncodingsByCode;

class EncodingListReader : public ZLXMLReader {

public:
	EncodingListReader(std::vector<std::string> &knownEncodings, std::map<int,std::string> &encodingsByCode);
	void startElementHandler(const char *tag, const char **attributes);

private:
	std::vector<std::string> &myKnownEncodings;
  std::map<int,std::string> &myEncodingsByCode;
};

EncodingListReader::EncodingListReader(std::vector<std::string> &knownEncodings, std::map<int,std::string> &encodingsByCode) : myKnownEncodings(knownEncodings), myEncodingsByCode(encodingsByCode) {
}

void EncodingListReader::startElementHandler(const char *tag, const char **attributes) {
	static const std::string ENCODING = "encoding";
	static const std::string NAME = "name";
	static const std::string CODE = "code";

	if (ENCODING == tag) {
		const char *name = attributeValue(attributes, NAME.c_str());
		if (name != 0) {
		  myKnownEncodings.push_back(name);
			const char *codeString = attributeValue(attributes, CODE.c_str());
			if (codeString != 0) {
				int code = atoi(codeString);
				if (code != 0) {
					myEncodingsByCode[code] = name;
				}
			}
	  }
	}
}

void ZLEncodingConverter::setEncodingDescriptionPath(const std::string &path) {
	ourKnownEncodings.clear();
	ourEncodingDescriptionPath = path;
	std::vector<std::string> candidates;
	EncodingListReader(candidates, ourEncodingsByCode).readDocument(ZLFile(ourEncodingDescriptionPath + "/Encodings.list").inputStream());
	for (std::vector<std::string>::const_iterator it = candidates.begin(); it != candidates.end(); it++) {
		iconv_t converter = iconv_open("utf-8", it->c_str());
		if (converter != (iconv_t)-1) {
			iconv_close(converter);
			ourKnownEncodings.push_back(*it);
		} else if (ZLFile(ourEncodingDescriptionPath + '/' + *it).exists()) {
			ourKnownEncodings.push_back(*it);
		}
	}
	if (ourKnownEncodings.empty()) {
		ourKnownEncodings.push_back("US-ASCII");
		ourKnownEncodings.push_back("UTF-8");
	}
	std::sort(ourKnownEncodings.begin(), ourKnownEncodings.end());
}
