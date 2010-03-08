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

#include "NetworkItems.h"

const ZLTypeId NetworkBookItem::TYPE_ID(NetworkItem::TYPE_ID);

bool NetworkBookItem::AuthorData::operator < (const AuthorData &data) const {
	const int sComp = SortKey.compare(data.SortKey);
	return (sComp < 0) || (sComp == 0 && DisplayName < data.DisplayName);
}

NetworkBookItem::NetworkBookItem(
	const NetworkLink &link,
	const std::string &id,
	unsigned int index,
	const std::string &title,
	const std::string &summary,
	const std::string &language,
	const std::string &date,
	const std::string &price,
	const std::vector<AuthorData> &authors,
	const std::vector<std::string> &tags,
	const std::string &seriesTitle,
	unsigned int indexInSeries,
	const std::map<URLType,std::string> &urlByType,
	const std::vector<shared_ptr<BookReference> > references
) : 
	NetworkItem(link, title, summary, urlByType),
	Index(index),
	Id(id),
	Language(language),
	Date(date),
	Price(price),
	Authors(authors),
	Tags(tags),
	SeriesTitle(seriesTitle),
	IndexInSeries(indexInSeries),
	myReferences(references) {
}

NetworkBookItem::NetworkBookItem(const NetworkBookItem &book, unsigned int index) :
	NetworkItem(book.Link, book.Title, book.Summary, book.URLByType), 
	Index(index), 
	Id(book.Id), 
	Language(book.Language), 
	Date(book.Date), 
	Price(book.Price), 
	Authors(book.Authors), 
	Tags(book.Tags),
	SeriesTitle(book.SeriesTitle),
	IndexInSeries(book.IndexInSeries) {
}

const ZLTypeId &NetworkBookItem::typeId() const {
	return TYPE_ID;
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

shared_ptr<BookReference> NetworkBookItem::reference(BookReference::Type type) const {
	shared_ptr<BookReference> reference;
	for (std::vector<shared_ptr<BookReference> >::const_iterator it = myReferences.begin(); it != myReferences.end(); ++it) {
		if ((*it)->ReferenceType == type &&
				(reference.isNull() || (*it)->BookFormat > reference->BookFormat)) {
			reference = *it;
		}
	}
	return reference;
}

shared_ptr<BookReference> NetworkBookItem::reference(BookReference::Format format, BookReference::Type type) const {
	for (std::vector<shared_ptr<BookReference> >::const_iterator it = myReferences.begin(); it != myReferences.end(); ++it) {
		if ((*it)->BookFormat == format && (*it)->ReferenceType == type) {
			return *it;
		}
	}
	return 0;
}
