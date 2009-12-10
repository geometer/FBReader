/*
 * Copyright (C) 2009 Geometer Plus <contact@geometerplus.com>
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

void NetworkBookCollection::addBook(shared_ptr<NetworkLibraryItem> bookPtr) {
	if (bookPtr.isNull() || bookPtr->typeId() != NetworkLibraryBookItem::TYPE_ID) {
		return;
	}
	myAuthorBooksMap.reset();

	NetworkLibraryItemList::iterator it = std::upper_bound(myBookList.begin(), myBookList.end(), bookPtr, NetworkBookItemComparator());
	myBookList.insert(it, bookPtr);

	NetworkLibraryBookItem &book = (NetworkLibraryBookItem &) *bookPtr;

	for (std::vector<NetworkLibraryBookItem::AuthorData>::iterator jt = book.authors().begin(); jt != book.authors().end(); ++jt) {
		NetworkLibraryBookItem::AuthorData &author = *jt;
		std::map<NetworkLibraryBookItem::AuthorData, unsigned int>::iterator kt = myAuthorRates.find(author);
		if (kt == myAuthorRates.end()) {
			myAuthorRates[author] = book.index();
		} else if (kt->second > book.index()) {
			kt->second = book.index();
		}
	}
}

const NetworkAuthorBooksMap &NetworkBookCollection::authorBooksMap() {
	if (myAuthorBooksMap.isNull()) {
		myAuthorBooksMap = new NetworkAuthorBooksMap(*myAuthorComparator);
		NetworkAuthorBooksMap &bookMap = *myAuthorBooksMap;
		for (NetworkLibraryItemList::const_iterator it = myBookList.begin(); it != myBookList.end(); ++it) {
			NetworkLibraryBookItem &book = (NetworkLibraryBookItem &) **it;
			for (std::vector<NetworkLibraryBookItem::AuthorData>::const_iterator jt = book.authors().begin(); jt != book.authors().end(); ++jt) {
				bookMap[*jt].push_back(*it);
			}
		}
	}
	return *myAuthorBooksMap;
}
