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

#include <string.h>
#include <stdlib.h>

#include <algorithm>

#include <ZLUnicodeUtil.h>
#include <ZLFileImage.h>

#include "ORBookReader.h"
#include "../xhtml/XHTMLReader.h"
#include "../util/MiscUtil.h"
#include "../../bookmodel/BookModel.h"

ORBookReader::ORBookReader(BookModel &model) : myModelReader(model) {
}

void ORBookReader::characterDataHandler(const char *data, int len) {
	if (myState == READ_TOCTITLE) {
		myTOCTitle.append(data, len);
	}
}

static const std::string TAG_RESOURCES = "resources";
static const std::string TAG_USERSET = "userset";
static const std::string TAG_NAVIGATION = "primarynav";

static const std::string TAG_SPINE = "spine";
static const std::string TAG_COVER = "cover";

static const std::string TAG_ITEM = "item";
static const std::string TAG_ITEMREF = "itemref";
static const std::string TAG_POINTER = "pointer";
static const std::string TAG_TITLE = "title";

static const std::string xhtmlMediaType = "application/x-orp-bcd1+xml";

void ORBookReader::startElementHandler(const char *tag, const char **xmlattributes) {
	const std::string tagString = ZLUnicodeUtil::toLower(tag);
	if (TAG_RESOURCES == tagString) {
		myState = READ_RESOURCES;
	} else if (TAG_USERSET == tagString) {
		myState = READ_USERSET;
	} else if ((myState == READ_RESOURCES) && (TAG_ITEM == tagString)) {
		const char *resid = attributeValue(xmlattributes, "resid");
		const char *resource = attributeValue(xmlattributes, "resource");
		const char *mediaType = attributeValue(xmlattributes, "media-type");
		if ((resid != 0) && (resource != 0)) {
			myResources[resid] = resource;
			if (mediaType != 0) {
				if (xhtmlMediaType == mediaType) {
					myHtmlFileIDs.insert(resid);
				} else if (std::string(mediaType, 6) == "image/") {
					myImageIDs[resid] = mediaType;
				}
			}
		}
	} else if (myState == READ_USERSET) {
		if (TAG_NAVIGATION == tagString) {
			myState = READ_NAVIGATION;
		} else if (TAG_SPINE == tagString) {
			const char *residrefs = attributeValue(xmlattributes, "residrefs");
			if (residrefs != 0) {
				while (1) {
					const char *nextSpace = strchr(residrefs, ' ');
					if (nextSpace == 0) {
						if (*residrefs != '\0') {
							myHtmlFilesOrder.push_back(residrefs);
						}
						break;
					}
					if (nextSpace != residrefs) {
						myHtmlFilesOrder.push_back(std::string(residrefs, nextSpace - residrefs));
					}
					residrefs = nextSpace + 1;
				}
			}
		} else if (TAG_COVER == tagString) {
			const char *residrefs = attributeValue(xmlattributes, "residrefs");
			if (residrefs != 0) {
				myCoverReference = residrefs;
			}
		}
	} else if ((myState == READ_NAVIGATION) && (TAG_POINTER == tagString)) {
		const char *ref = attributeValue(xmlattributes, "elemrefs");
		const char *level = attributeValue(xmlattributes, "level");
		if ((ref != 0) && (level != 0)) {
			myTOCReference = ref;
			myTOCLevel = atoi(level);
			myState = READ_POINTER;
		}
	} else if ((myState == READ_POINTER) && (TAG_TITLE == tagString)) {
		myState = READ_TOCTITLE;
	}
}

void ORBookReader::endElementHandler(const char *tag) {
	const std::string tagString = ZLUnicodeUtil::toLower(tag);
	if ((TAG_RESOURCES == tagString) || (TAG_USERSET == tagString)) {
		myState = READ_NONE;
	} else if ((myState == READ_NAVIGATION) && (TAG_NAVIGATION == tagString)) {
		myState = READ_USERSET;
	} else if ((myState == READ_POINTER) && (TAG_POINTER == tagString)) {
		myState = READ_NAVIGATION;
	} else if ((myState == READ_TOCTITLE) && (TAG_TITLE == tagString)) {
		myTOC.push_back(TOCItem(myTOCReference, myTOCTitle, myTOCLevel));
		myTOCTitle.erase();
		myState = READ_POINTER;
	}
}

bool ORBookReader::readBook(const std::string &fileName) {
	myFilePrefix = MiscUtil::htmlDirectoryPrefix(fileName);

	myResources.clear();
	myCoverReference.erase();
	myHtmlFileIDs.clear();
	myImageIDs.clear();
	myHtmlFilesOrder.clear();
	myTOC.clear();
	myState = READ_NONE;

	if (!readDocument(fileName)) {
		return false;
	}

	myModelReader.setMainTextModel();
	myModelReader.pushKind(REGULAR);

	if (!myCoverReference.empty()) {
		myModelReader.addImageReference(myCoverReference);
	}

	for (std::vector<std::string>::const_iterator it = myHtmlFilesOrder.begin(); it != myHtmlFilesOrder.end(); ++it) {
		myHtmlFileIDs.erase(*it);
		XHTMLReader(myModelReader).readFile(myFilePrefix, myResources[*it], *it);
	}

	int level = 1;
	for (std::vector<TOCItem>::const_iterator it = myTOC.begin(); it != myTOC.end(); ++it) {
		int index = myModelReader.model().label(it->Reference).ParagraphNumber;
		if (index != -1) {
			for (; level > it->Level; --level) {
				myModelReader.endContentsParagraph();
			}
			++level;
			myModelReader.beginContentsParagraph(index);
			myModelReader.addContentsData(it->Text);
		}
	}
	for (; level > 1; --level) {
		myModelReader.endContentsParagraph();
	}

	for (std::set<std::string>::const_iterator it = myHtmlFileIDs.begin(); it != myHtmlFileIDs.end(); ++it) {
		myModelReader.setFootnoteTextModel(*it);
		myModelReader.pushKind(REGULAR);
		XHTMLReader(myModelReader).readFile(myFilePrefix, myResources[*it], *it);
	}

	for (std::map<std::string,std::string>::const_iterator it = myImageIDs.begin(); it != myImageIDs.end(); ++it) {
		myModelReader.addImage(it->first, new ZLFileImage(it->second, myFilePrefix + myResources[it->first], 0));
	}

	return true;
}
