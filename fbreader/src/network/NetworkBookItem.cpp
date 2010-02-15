/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
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

//#include <ZLStringUtil.h>
//#include <ZLFile.h>

#include "NetworkItems.h"
#include "NetworkAuthenticationManager.h"

const ZLTypeId NetworkBookItem::TYPE_ID(NetworkItem::TYPE_ID);

bool NetworkBookItem::AuthorData::operator < (const AuthorData &data) const {
	const int sComp = SortKey.compare(data.SortKey);
	return (sComp < 0) || (sComp == 0 && DisplayName < data.DisplayName);
}

NetworkBookItem::NetworkBookItem(
	const std::string &id,
	unsigned int index,
	const std::string &title,
	const std::string &summary,
	const std::string &coverURL,
	const std::string &language,
	const std::map<URLType,std::string> urlByType
) : 
	NetworkItem(title, summary, coverURL, urlByType),
	myIndex(index),
	myId(id),
	myLanguage(language) {
}

NetworkBookItem::NetworkBookItem(const NetworkBookItem &book) :
	NetworkItem(book.Title, book.Summary, book.CoverURL, book.URLByType), 
	myIndex(book.myIndex), 
	myId(book.myId), 
	myLanguage(book.myLanguage), 
	myDate(book.myDate), 
	mySeriesTitle(book.mySeriesTitle), 
	myIndexInSeries(book.myIndexInSeries),
	myPrice(book.myPrice), 
	myAuthors(book.myAuthors), 
	myTags(book.myTags), 
	myAuthenticationManager(book.myAuthenticationManager) {
}

const ZLTypeId &NetworkBookItem::typeId() const {
	return TYPE_ID;
}

const std::string &NetworkBookItem::id() const {
	return myId;
}

/*std::string NetworkBookItem::fileName(URLType format) const {
	std::string authorName;
	size_t maxSize = 256 - title().size() - myLanguage.size() - 4;
	if (!myAuthors.empty()) {
		authorName = myAuthors[0].DisplayName;
		for (size_t i = 1;  i < myAuthors.size() && authorName.size() < maxSize; ++i) {
			authorName.append(",_");
			authorName.append(myAuthors[i].DisplayName);
		}
	}
	if (authorName.size() > maxSize) {
		authorName.erase(maxSize);
		ZLStringUtil::stripWhiteSpaces(authorName);
	}
	std::string fName = authorName + "_" + title() + "_(" + myLanguage + ")";
	switch (format) {
		case LINK_HTTP:
			break;
		case BOOK_EPUB:
			fName += ".epub";
			break;
		case BOOK_MOBIPOCKET:
			fName += ".mobi";
			break;
		case BOOK_FB2_ZIP:
			fName += ".fb2.zip";
			break;
//		case BOOK_PDF:
//			fName += ".pdf";
//			break;
		case BOOK_DEMO_FB2_ZIP:
			fName += "_(trial).fb2.zip";
			break;
		case NONE:
			return "";
			break;
	}
	return ZLFile::replaceIllegalCharacters(fName, '_');
}*/

NetworkItem::URLType NetworkBookItem::bestBookFormat() const {
	if (URLByType.count(BOOK_EPUB) != 0) {
		return BOOK_EPUB;
	} else if (URLByType.count(BOOK_FB2_ZIP) != 0) {
		return BOOK_FB2_ZIP;
	} else if (URLByType.count(BOOK_MOBIPOCKET) != 0) {
		return BOOK_MOBIPOCKET;
	}
	return NONE;
}

NetworkItem::URLType NetworkBookItem::bestDemoFormat() const {
	if (URLByType.count(BOOK_DEMO_FB2_ZIP) != 0) {
		return BOOK_DEMO_FB2_ZIP;
	}
	return NONE;
}

shared_ptr<NetworkAuthenticationManager> NetworkBookItem::authenticationManager() const {
	return myAuthenticationManager;
}

void NetworkBookItem::setAuthenticationManager(shared_ptr<NetworkAuthenticationManager> manager) {
	myAuthenticationManager = manager;
}

void NetworkBookItem::setLanguage(const std::string &language) {
	myLanguage = language;
}

void NetworkBookItem::setDate(const std::string &date) {
	myDate = date;
}

void NetworkBookItem::setPrice(const std::string &price) {
	myPrice = price;
}

void NetworkBookItem::setSeries(const std::string &title, int index) {
	mySeriesTitle = title;
	myIndexInSeries = index;
}
