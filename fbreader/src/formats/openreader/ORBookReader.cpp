/*
 * FBReader -- electronic book reader
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
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

#include <iostream>
#include <algorithm>

#include <ZLUnicodeUtil.h>

#include "ORBookReader.h"
#include "../oeb/XHTMLReader.h"
#include "../../bookmodel/BookModel.h"

ORBookReader::ORBookReader(BookModel &model) : myModelReader(model) {
}

void ORBookReader::characterDataHandler(const char*, int) {
}

static const std::string RESOURCES = "resources";
static const std::string NAVIGATION = "primarynav";

static const std::string ITEM = "item";
static const std::string ITEMREF = "itemref";
static const std::string POINTER = "pointer";

static const std::string xhtmlMediaType = "application/x-orp-bcd1+xml";

void ORBookReader::startElementHandler(const char *tag, const char **xmlattributes) {
	std::string tagString = ZLUnicodeUtil::toLower(tag);
	if (RESOURCES == tagString) {
		myState = READ_RESOURCES;
	} else if (NAVIGATION == tagString) {
		myState = READ_NAVIGATION;
	} else if ((myState == READ_RESOURCES) && (ITEM == tagString)) {
		const char *resid = attributeValue(xmlattributes, "resid");
		const char *resource = attributeValue(xmlattributes, "resource");
		const char *mediaType = attributeValue(xmlattributes, "media-type");
		if ((resid != 0) && (resource != 0)) {
			myResources[resid] = resource;
			if ((mediaType != 0) && (xhtmlMediaType == mediaType)) {
				myHtmlFileIDs.push_back(resid);
			}
		}
	} else if ((myState == READ_NAVIGATION) && (POINTER == tagString)) {
		const char *title = attributeValue(xmlattributes, "targetclass");
		const char *href = attributeValue(xmlattributes, "elemrefs");
		if ((title != 0) && (href != 0)) {
			myTOC.push_back(std::pair<std::string,std::string>(title, href));
		}
	}
}

void ORBookReader::endElementHandler(const char *tag) {
	std::string tagString = ZLUnicodeUtil::toLower(tag);
	if ((RESOURCES == tagString) || (NAVIGATION == tagString)) {
		myState = READ_NONE;
	}
}

bool ORBookReader::readBook(const std::string &fileName) {
	int index0 = fileName.rfind(':');
	int index1 = fileName.rfind('/');
	myFilePrefix = fileName.substr(0, std::max(index0, index1) + 1);

	myResources.clear();
	myHtmlFileIDs.clear();
	myTOC.clear();
	myState = READ_NONE;

	if (!readDocument(fileName)) {
		return false;
	}

	myModelReader.setMainTextModel();
	myModelReader.pushKind(REGULAR);

	for (std::vector<std::string>::const_iterator it = myHtmlFileIDs.begin(); it != myHtmlFileIDs.end(); ++it) {
		XHTMLReader(myModelReader).readFile(myFilePrefix, myResources[*it], *it);
	}

	for (std::vector<std::pair<std::string, std::string> >::const_iterator it = myTOC.begin(); it != myTOC.end(); ++it) {
		std::cerr << it->first << "\n";
		int index = myModelReader.model().paragraphNumberById(it->second);
		if (index != -1) {
			myModelReader.beginContentsParagraph(index);
			myModelReader.addContentsData(it->first);
			myModelReader.endContentsParagraph();
		}
	}

	return true;
}
