/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

#include <ZLStringUtil.h>
#include <ZLUnicodeUtil.h>

#include "OEBMigrationReader.h"

OEBMigrationReader::OEBMigrationReader(BookInfo &info) : myInfo(info) {
}

static const std::string METADATA = "metadata";
static const std::string DC_METADATA = "dc-metadata";
static const std::string SUBJECT_TAG = ":subject";

void OEBMigrationReader::characterDataHandler(const char *text, int len) {
	if (myReadSubject) {
		myBuffer.append(text, len);
	}
}

bool OEBMigrationReader::isDublinCoreNamespace(const std::string &nsId) const {
	static const std::string DC_SCHEME_PREFIX = "http://purl.org/dc/elements";
	const std::map<std::string,std::string> &namespaceMap = namespaces();
	std::map<std::string,std::string>::const_iterator iter = namespaceMap.find(nsId);
	return
		(iter != namespaceMap.end()) &&
		ZLStringUtil::stringStartsWith(iter->second, DC_SCHEME_PREFIX);
}

void OEBMigrationReader::startElementHandler(const char *tag, const char**) {
	const std::string tagString = ZLUnicodeUtil::toLower(tag);
	if ((METADATA == tagString) || (DC_METADATA == tagString)) {
		myDCMetadataTag = tagString;
		myReadMetaData = true;
	} else if (myReadMetaData) {
		if (ZLStringUtil::stringEndsWith(tagString, SUBJECT_TAG)) {
			if (isDublinCoreNamespace(tagString.substr(0, tagString.length() - SUBJECT_TAG.length()))) {
				myReadSubject = true;
			}
		}
	}
}

void OEBMigrationReader::endElementHandler(const char *tag) {
	const std::string tagString = ZLUnicodeUtil::toLower(tag);
	if (myDCMetadataTag == tagString) {
		interrupt();
	} else if (myReadSubject) {
		ZLStringUtil::stripWhiteSpaces(myBuffer);
		if (!myBuffer.empty()) {
			if (!myTagList.empty()) {
				myTagList += ',';
			}
			myTagList += myBuffer;
			myBuffer.erase();
		}
		myReadSubject = false;
	}
}

bool OEBMigrationReader::processNamespaces() const {
	return true;
}

void OEBMigrationReader::doRead(const std::string &fileName) {
	myReadMetaData = false;
	myReadSubject = false;
	readDocument(fileName);
	myInfo.TagsOption.setValue(myTagList);
}
