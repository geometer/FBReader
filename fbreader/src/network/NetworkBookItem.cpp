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
#include "NetworkLink.h"
#include "authentication/NetworkAuthenticationManager.h"

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
	Authors(book.Authors), 
	Tags(book.Tags),
	SeriesTitle(book.SeriesTitle),
	IndexInSeries(book.IndexInSeries) {
}

const ZLTypeId &NetworkBookItem::typeId() const {
	return TYPE_ID;
}

shared_ptr<BookReference> NetworkBookItem::reference(BookReference::Type type) const {
	shared_ptr<BookReference> reference;
	for (std::vector<shared_ptr<BookReference> >::const_iterator it = myReferences.begin(); it != myReferences.end(); ++it) {
		if ((*it)->ReferenceType == type &&
				(reference.isNull() || (*it)->BookFormat > reference->BookFormat)) {
			reference = *it;
		}
	}

	if (reference.isNull() && type == BookReference::DOWNLOAD_FULL) {
		reference = this->reference(BookReference::DOWNLOAD_FULL_CONDITIONAL);
		if (!reference.isNull()) {
			shared_ptr<NetworkAuthenticationManager> authManager =
				Link.authenticationManager();
			if (authManager.isNull() || authManager->needPurchase(*this)) {
				return 0;
			}
			reference = authManager->downloadReference(*this);
		}
	}

	if (reference.isNull() &&
			type == BookReference::DOWNLOAD_FULL &&
			this->reference(BookReference::BUY).isNull() &&
			this->reference(BookReference::BUY_IN_BROWSER).isNull()) {
		reference = this->reference(BookReference::DOWNLOAD_FULL_OR_DEMO);
	}

	if (reference.isNull() &&
			type == BookReference::DOWNLOAD_DEMO &&
			(!this->reference(BookReference::BUY).isNull() ||
			 !this->reference(BookReference::BUY_IN_BROWSER).isNull())) {
		reference = this->reference(BookReference::DOWNLOAD_FULL_OR_DEMO);
	}

	return reference;
}

std::string NetworkBookItem::localCopyFileName() const {
	const bool hasBuyReference =
		!this->reference(BookReference::BUY).isNull() ||
		!this->reference(BookReference::BUY_IN_BROWSER).isNull();
	shared_ptr<BookReference> reference;
	std::string fileName;
	for (std::vector<shared_ptr<BookReference> >::const_iterator it = myReferences.begin(); it != myReferences.end(); ++it) {
		const BookReference::Type type = (*it)->ReferenceType;
		if ((type == BookReference::DOWNLOAD_FULL ||
				 type == BookReference::DOWNLOAD_FULL_CONDITIONAL ||
				 (!hasBuyReference && type == BookReference::DOWNLOAD_FULL_OR_DEMO)) &&
				(reference.isNull() || (*it)->BookFormat > reference->BookFormat)) {
			std::string name = (*it)->localCopyFileName();
			if (!name.empty()) {
				reference = *it;
				fileName = name;
			}
		}
	}
	return fileName;
}

void NetworkBookItem::removeLocalFiles() const {
	const bool hasBuyReference =
		!this->reference(BookReference::BUY).isNull() ||
		!this->reference(BookReference::BUY_IN_BROWSER).isNull();
	for (std::vector<shared_ptr<BookReference> >::const_iterator it = myReferences.begin(); it != myReferences.end(); ++it) {
		const BookReference::Type type = (*it)->ReferenceType;
		if (type == BookReference::DOWNLOAD_FULL ||
				type == BookReference::DOWNLOAD_FULL_CONDITIONAL ||
				(!hasBuyReference && type == BookReference::DOWNLOAD_FULL_OR_DEMO)) {
			std::string fileName = (*it)->localCopyFileName();
			if (!fileName.empty()) {
				// TODO: remove a book from the library
				// TODO: remove a record from the database
				ZLFile(fileName).remove();
			}
		}
	}
}
