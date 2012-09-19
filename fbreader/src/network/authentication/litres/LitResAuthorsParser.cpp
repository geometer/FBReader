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

#include <cstdlib>

#include <ZLStringUtil.h>

#include "../../NetworkLink.h"
#include "LitResGenre.h"
#include "LitResUtil.h"
#include "LitResCatalogItem.h"


#include "LitResAuthorsParser.h"

static const std::string TAG_CATALOG = "catalit-persons";
static const std::string TAG_SUBJECT = "subject";
static const std::string TAG_TITLE = "title";
static const std::string TAG_MAIN = "main";
static const std::string TAG_FIRST_NAME = "first-name";
static const std::string TAG_MIDDLE_NAME = "middle-name";
static const std::string TAG_LAST_NAME = "last-name";

std::string LitResAuthorsParser::stringAttributeValue(const char **attributes, const char *name) {
	if (attributes == 0) {
		return std::string();
	}
	const char *value = attributeValue(attributes, name);
	return value != 0 ? value : std::string();
}

LitResAuthorsParser::LitResAuthorsParser(
	const NetworkLink &link,
	const std::map<NetworkItem::URLType,std::string> &urlByType,
	NetworkItem::List &authors) :
	myLink(link),
	myUrlByType(urlByType),
	myAuthors(authors) {
	myState = START;
}


void LitResAuthorsParser::startElementHandler(const char *tag, const char **attributes) {
	processState(tag, false, attributes);
	myState = getNextState(tag, false);
	myBuffer.clear();
}

void LitResAuthorsParser::endElementHandler(const char *tag) {
	processState(tag, true, 0);
	myState = getNextState(tag, true);
	myBuffer.clear();
}

void LitResAuthorsParser::characterDataHandler(const char *data, size_t len) {
	myBuffer.append(data, len);
}

void LitResAuthorsParser::processState(const std::string &tag, bool closed, const char **attributes) {
	switch(myState) {
	case START:
			break;
	case CATALOG:
		if (!closed && TAG_SUBJECT == tag) {
			myAuthorId = stringAttributeValue(attributes, "id");
		}
		break;
	case SUBJECT:
		if (closed && TAG_SUBJECT == tag) {
			std::map<NetworkItem::URLType,std::string> urlByType = myUrlByType;
			urlByType[NetworkItem::URL_CATALOG] = LitResUtil::generateBooksByAuthorUrl(myAuthorId);
			//TODO add icon change for one author here
			//urlByType[NetworkItem::URL_COVER] =
			myAuthors.push_back(new LitResCatalogItem(
				true,
				myLink,
				myAuthorDisplayName,
				getSubtitle(),
				urlByType,
				NetworkCatalogItem::Always,
				NetworkCatalogItem::FLAG_GROUP_MORE_THAN_1_BOOK_BY_SERIES
			));

			myAuthorId.clear();
			myAuthorDisplayName.clear();
			myAuthorFirstName.clear();
			myAuthorMiddleName.clear();
			myAuthorLastName.clear();
		}
		break;
	case TITLE:
		break;
	case MAIN:
		if (closed && TAG_MAIN == tag) {
			ZLStringUtil::stripWhiteSpaces(myBuffer);
			myAuthorDisplayName = myBuffer;
		}
		break;
	case FIRST_NAME:
		if (closed && TAG_FIRST_NAME == tag) {
			ZLStringUtil::stripWhiteSpaces(myBuffer);
			myAuthorFirstName = myBuffer;
		}
		break;
	case MIDDLE_NAME:
		if (closed && TAG_MIDDLE_NAME == tag) {
			ZLStringUtil::stripWhiteSpaces(myBuffer);
			myAuthorMiddleName = myBuffer;
		}
		break;
	case LAST_NAME:
		if (closed && TAG_LAST_NAME == tag) {
			ZLStringUtil::stripWhiteSpaces(myBuffer);
			myAuthorLastName = myBuffer;
		}
		break;
	}
}

LitResAuthorsParser::State LitResAuthorsParser::getNextState(const std::string &tag, bool closed) {
	switch(myState) {
	case START:
		if (!closed && TAG_CATALOG == tag) {
			return CATALOG;
		}
		break;
	case CATALOG:
		if (!closed) {
			if (TAG_SUBJECT == tag) {
				return SUBJECT;
			}
		} else {
			if (TAG_CATALOG == tag) {
				return START;
			}
		}
		break;
	case SUBJECT:
		if (!closed) {
			if (TAG_TITLE == tag) {
				return TITLE;
			}
		} else {
			if (TAG_SUBJECT == tag) {
				return CATALOG;
			}
		}
		break;
	case TITLE:
		if (!closed) {
			if (TAG_MAIN == tag) {
				return MAIN;
			}
		} else {
			if (TAG_TITLE == tag) {
				return FIRST_NAME;
			}
		}
		break;
	case MAIN:
		if (closed && TAG_MAIN == tag) {
				return TITLE;
		}
		break;
	case FIRST_NAME:
		if (closed && TAG_FIRST_NAME == tag) {
			return MIDDLE_NAME;
		}
		break;
	case MIDDLE_NAME:
		if (closed && TAG_MIDDLE_NAME == tag) {
			return LAST_NAME;
		}
		break;
	case LAST_NAME:
		if (closed && TAG_LAST_NAME == tag) {
			return SUBJECT;
		}
		break;
	}
	return myState;
}

std::string LitResAuthorsParser::getSubtitle() const {
	static const std::string SPACE = " ";
	std::string subtitle = myAuthorFirstName;
	if (!myAuthorMiddleName.empty()) {
		if (!subtitle.empty()) {
			subtitle += SPACE;
		}
		subtitle += myAuthorMiddleName;
	}
	if (!myAuthorLastName.empty()) {
		if (!subtitle.empty()) {
			subtitle += SPACE;
		}
		subtitle += myAuthorLastName;
	}
	return subtitle;
}
