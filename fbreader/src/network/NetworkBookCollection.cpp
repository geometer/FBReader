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

#include <algorithm>

#include <ZLResource.h>

#include "NetworkBookCollection.h"


NetworkBookCollection::NetworkBookCollection() {
	myAuthorComparator = new NetworkAuthorComparator(myAuthorRates);
}

void NetworkBookCollection::addBook(shared_ptr<NetworkItem> bookPtr) {
	if (bookPtr.isNull() || bookPtr->typeId() != NetworkBookItem::TYPE_ID) {
		return;
	}
	myAuthorBooksMap.reset();

	NetworkItem::List::iterator it = std::upper_bound(myBookList.begin(), myBookList.end(), bookPtr, NetworkBookItemComparator());
	myBookList.insert(it, bookPtr);

	NetworkBookItem &book = (NetworkBookItem &) *bookPtr;

	for (std::vector<NetworkBookItem::AuthorData>::const_iterator jt = book.Authors.begin(); jt != book.Authors.end(); ++jt) {
		const NetworkBookItem::AuthorData &author = *jt;
		std::map<NetworkBookItem::AuthorData, unsigned int>::iterator kt = myAuthorRates.find(author);
		if (kt == myAuthorRates.end()) {
			myAuthorRates[author] = book.Index;
		} else if (kt->second > book.Index) {
			kt->second = book.Index;
		}
	}
}

const NetworkAuthorBooksMap &NetworkBookCollection::authorBooksMap() {
	if (myAuthorBooksMap.isNull()) {
		myAuthorBooksMap = new NetworkAuthorBooksMap(*myAuthorComparator);
		NetworkAuthorBooksMap &bookMap = *myAuthorBooksMap;
		for (NetworkItem::List::const_iterator it = myBookList.begin(); it != myBookList.end(); ++it) {
			NetworkBookItem &book = (NetworkBookItem &) **it;
			for (std::vector<NetworkBookItem::AuthorData>::const_iterator jt = book.Authors.begin(); jt != book.Authors.end(); ++jt) {
				bookMap[*jt].push_back(*it);
			}
		}
	}
	return *myAuthorBooksMap;
}
