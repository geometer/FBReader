/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
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

#include <ZLUnicodeUtil.h>
#include <ZLXMLNamespace.h>

#include "OEBMigrationReader.h"

OEBMigrationReader::OEBMigrationReader(BookInfo &info) : myInfo(info) {
}

static const std::string METADATA = "metadata";
static const std::string DC_METADATA = "dc-metadata";

void OEBMigrationReader::characterDataHandler(const char *text, std::size_t len) {
	if (myReadSubject) {
		myBuffer.append(text, len);
	}
}

bool OEBMigrationReader::testDCTag(const std::string &name, const std::string &tag) const {
	return
		testTag(ZLXMLNamespace::DublinCore, name, tag) ||
		testTag(ZLXMLNamespace::DublinCoreLegacy, name, tag);
}

void OEBMigrationReader::startElementHandler(const char *tag, const char**) {
	const std::string tagString = ZLUnicodeUtil::toLower(tag);
	if ((METADATA == tagString) || (DC_METADATA == tagString)) {
		myDCMetadataTag = tagString;
		myReadMetaData = true;
	} else if (myReadMetaData) {
		if (testDCTag("subject", tagString)) {
				myReadSubject = true;
		}
	}
}

void OEBMigrationReader::endElementHandler(const char *tag) {
	const std::string tagString = ZLUnicodeUtil::toLower(tag);
	if (myDCMetadataTag == tagString) {
		interrupt();
	} else if (myReadSubject) {
		ZLUnicodeUtil::utf8Trim(myBuffer);
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

void OEBMigrationReader::doRead(const ZLFile &file) {
	myReadMetaData = false;
	myReadSubject = false;
	readDocument(file);
	myInfo.TagsOption.setValue(myTagList);
}
