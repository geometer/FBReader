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

class EncodingListReader : public ZLXMLReader {

protected:
	const Tag *tags() const;

public:
	EncodingListReader(std::vector<std::string> &knownEncodings);
	void startElementHandler(int tag, const char **attributes);
	void endElementHandler(int) {}
	void characterDataHandler(const char *, int) {}

private:
	std::vector<std::string> &myKnownEncodings;
};

EncodingListReader::EncodingListReader(std::vector<std::string> &knownEncodings) : myKnownEncodings(knownEncodings) {
}

static const ZLXMLReader::Tag TAGS[] = {
	{ "known-encodings", 0 },
	{ "encoding", 1 },
	{ 0, 2 }
};

const ZLXMLReader::Tag *EncodingListReader::tags() const {
	return TAGS;
}

static const std::string NAME = "name";

void EncodingListReader::startElementHandler(int tag, const char **attributes) {
	if ((tag == 1) && (attributes[0] != 0) && (NAME == attributes[0])) {
		myKnownEncodings.push_back(attributes[1]);
	}
}

void ZLEncodingConverter::setEncodingDescriptionPath(const std::string &path) {
	ourKnownEncodings.clear();
	ourEncodingDescriptionPath = path;
	std::vector<std::string> candidates;
	EncodingListReader(candidates).readDocument(ZLFile(ourEncodingDescriptionPath + "/Encodings.list").inputStream());
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
