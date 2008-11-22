/*
 * Copyright (C) 2008 Geometer Plus <contact@geometerplus.com>
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

#include "FBReaderOrgDataParser.h"
#include "../../constants/XMLNamespace.h"

FBReaderOrgDataParser::FBReaderOrgDataParser(NetworkBookList &books) : myBooks(books) {
}

bool FBReaderOrgDataParser::processNamespaces() const {
	return true;
}

void FBReaderOrgDataParser::namespaceListChangedHandler() {
	myDCPrefix.erase();
	const std::map<std::string,std::string> &nsMap = namespaces();
	for (std::map<std::string,std::string>::const_iterator it = nsMap.begin(); it != nsMap.end(); ++it) {
		if (it->second == XMLNamespace::DublinCore) {
			myDCPrefix = it->first + ':';
			break;
		}
	}
}

static const std::string BOOK_TAG = "book";
static const std::string COVER_TAG = "cover";
static const std::string SERIES_TAG = "series";
static const std::string BUY_URL_TAG = "buyURL";

static const std::string SUBJECT_DC_TAG = "subject";
static const std::string TITLE_DC_TAG = "title";
static const std::string AUTHOR_DC_TAG = "author";
static const std::string DATE_DC_TAG = "date";

void FBReaderOrgDataParser::startElementHandler(const char *tag, const char **attributes) {
	if (BOOK_TAG == tag) {
		const char *id = attributeValue(attributes, "id");
		if (id != 0) {
			myCurrentBook = new NetworkBookInfo(id);
		}
	}
	myBuffer.clear();
}

void FBReaderOrgDataParser::endElementHandler(const char *tag) {
	if (!myCurrentBook.isNull()) {
		const std::string sTag = tag;
		if (!myDCPrefix.empty() &&
				ZLStringUtil::stringStartsWith(sTag, myDCPrefix)) {
			const std::string dcTag = sTag.substr(myDCPrefix.length());
			if (SUBJECT_DC_TAG == dcTag) {
				myCurrentBook->Tags.push_back(myBuffer);
			} else if (TITLE_DC_TAG == dcTag) {
				myCurrentBook->Title = myBuffer;
			} else if (AUTHOR_DC_TAG == dcTag) {
				myCurrentBook->Author.DisplayName = myBuffer;
				int index = myBuffer.rfind(' ');
				if (index >= 0) {
					myCurrentBook->Author.SortKey = myBuffer.substr(index + 1);
				} else {
					myCurrentBook->Author.SortKey = myBuffer;
				}
			} else if (DATE_DC_TAG == dcTag) {
				myCurrentBook->Date = myBuffer;
			}
		} else {
			if (BOOK_TAG == sTag) {
				myBooks.push_back(myCurrentBook);
				myCurrentBook.reset();
			} else if (COVER_TAG == sTag) {
				myCurrentBook->Cover = myBuffer;
			} else if (SERIES_TAG == sTag) {
				myCurrentBook->Series = myBuffer;
			} else if (BUY_URL_TAG == sTag) {
				myCurrentBook->URLByType[NetworkBookInfo::LINK_HTTP] = myBuffer;
			}
		}
	}

	myBuffer.clear();
}

void FBReaderOrgDataParser::characterDataHandler(const char *data, size_t len) {
	myBuffer.append(data, len);
}
