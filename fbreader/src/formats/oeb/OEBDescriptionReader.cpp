/*
 * Copyright (C) 2004-2009 Geometer Plus <contact@geometerplus.com>
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

#include "OEBDescriptionReader.h"
#include "../../constants/XMLNamespace.h"

OEBDescriptionReader::OEBDescriptionReader(BookDescription &description) : myDescription(description) {
	myDescription.clearAuthor();
	myDescription.title().erase();
	myDescription.removeAllTags();
}

static const std::string METADATA = "metadata";
static const std::string DC_METADATA = "dc-metadata";
static const std::string METADATA_SUFFIX = ":metadata";
static const std::string TITLE_SUFFIX = ":title";
static const std::string AUTHOR_SUFFIX = ":creator";
static const std::string SUBJECT_SUFFIX = ":subject";
static const std::string LANGUAGE_SUFFIX = ":language";
static const std::string AUTHOR_ROLE = "aut";

void OEBDescriptionReader::characterDataHandler(const char *text, size_t len) {
	switch (myReadState) {
		case READ_NONE:
			break;
		case READ_AUTHOR:
		case READ_AUTHOR2:
		case READ_SUBJECT:
		case READ_LANGUAGE:
			myBuffer.append(text, len);
			break;
		case READ_TITLE:
			myDescription.title().append(text, len);
			break;
	}
}

bool OEBDescriptionReader::isDublinCoreNamespace(const std::string &nsId) const {
	const std::map<std::string,std::string> &namespaceMap = namespaces();
	std::map<std::string,std::string>::const_iterator iter = namespaces().find(nsId);
	return
		((iter != namespaceMap.end()) &&
		 (ZLStringUtil::stringStartsWith(iter->second, XMLNamespace::DublinCorePrefix) ||
		  ZLStringUtil::stringStartsWith(iter->second, XMLNamespace::DublinCoreLegacyPrefix)));
}

bool OEBDescriptionReader::isOPFNamespace(const std::string &nsId) const {
	const std::map<std::string,std::string> &namespaceMap = namespaces();
	std::map<std::string,std::string>::const_iterator iter = namespaceMap.find(nsId);
	return
		(iter != namespaceMap.end()) &&
		(iter->second == XMLNamespace::OpenPackagingFormat);
}

void OEBDescriptionReader::startElementHandler(const char *tag, const char **attributes) {
	const std::string tagString = ZLUnicodeUtil::toLower(tag);
	if ((METADATA == tagString) || (DC_METADATA == tagString) ||
			(ZLStringUtil::stringEndsWith(tagString, METADATA_SUFFIX) &&
			 isOPFNamespace(tagString.substr(0, tagString.length() - METADATA_SUFFIX.length())))) {
		myDCMetadataTag = tagString;
		myReadMetaData = true;
	} else if (myReadMetaData) {
		if (ZLStringUtil::stringEndsWith(tagString, TITLE_SUFFIX)) {
			if (isDublinCoreNamespace(tagString.substr(0, tagString.length() - TITLE_SUFFIX.length()))) {
				myReadState = READ_TITLE;
			}
		} else if (ZLStringUtil::stringEndsWith(tagString, AUTHOR_SUFFIX)) {
			if (isDublinCoreNamespace(tagString.substr(0, tagString.length() - AUTHOR_SUFFIX.length()))) {
				const char *role = attributeValue(attributes, "role");
				if (role == 0) {
					myReadState = READ_AUTHOR2;
				} else if (AUTHOR_ROLE == role) {
					myReadState = READ_AUTHOR;
				}
			}
		} else if (ZLStringUtil::stringEndsWith(tagString, SUBJECT_SUFFIX)) {
			if (isDublinCoreNamespace(tagString.substr(0, tagString.length() - SUBJECT_SUFFIX.length()))) {
				myReadState = READ_SUBJECT;
			}
		} else if (ZLStringUtil::stringEndsWith(tagString, LANGUAGE_SUFFIX)) {
			if (isDublinCoreNamespace(tagString.substr(0, tagString.length() - LANGUAGE_SUFFIX.length()))) {
				myReadState = READ_LANGUAGE;
			}
		}
	}
}

void OEBDescriptionReader::endElementHandler(const char *tag) {
	const std::string tagString = ZLUnicodeUtil::toLower(tag);
	if (myDCMetadataTag == tagString) {
		interrupt();
	} else {
		ZLStringUtil::stripWhiteSpaces(myBuffer);
		if (!myBuffer.empty()) {
			if (myReadState == READ_AUTHOR) {
				myAuthorList.push_back(myBuffer);
			} else if (myReadState == READ_AUTHOR2) {
				myAuthorList2.push_back(myBuffer);
			} else if (myReadState == READ_SUBJECT) {
				myDescription.addTag(myBuffer);
			} else if (myReadState == READ_LANGUAGE) {
				int index = myBuffer.find('-');
				if (index >= 0) {
					myBuffer = myBuffer.substr(0, index);
				}
				index = myBuffer.find('_');
				if (index >= 0) {
					myBuffer = myBuffer.substr(0, index);
				}
				if (myBuffer == "cz") {
					myBuffer = "cs";
				}
				myDescription.language() = myBuffer;
			}
			myBuffer.erase();
		}
		myReadState = READ_NONE;
	}
}

bool OEBDescriptionReader::processNamespaces() const {
	return true;
}

bool OEBDescriptionReader::readDescription(const std::string &fileName) {
	myReadMetaData = false;
	myReadState = READ_NONE;
	bool code = readDocument(fileName);
	if (code) {
		if (!myAuthorList.empty()) {
			for (std::vector<std::string>::const_iterator it = myAuthorList.begin(); it != myAuthorList.end(); ++it) {
				myDescription.addAuthor(*it);
			}
		} else {
			for (std::vector<std::string>::const_iterator it = myAuthorList2.begin(); it != myAuthorList2.end(); ++it) {
				myDescription.addAuthor(*it);
			}
		}
	}
	return code;
}
