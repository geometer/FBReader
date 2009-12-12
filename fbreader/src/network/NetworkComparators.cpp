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

#include "NetworkComparators.h"


bool NetworkBookItemComparator::operator () (const shared_ptr<NetworkLibraryItem> &bookPtr0, const shared_ptr<NetworkLibraryItem> &bookPtr1) {
	std::string book0Type = bookPtr0->typeId();
	std::string book1Type = bookPtr1->typeId();

	if (book0Type != NetworkLibraryBookItem::TYPE_ID && book1Type != NetworkLibraryBookItem::TYPE_ID) {
		return bookPtr0->title() < bookPtr1->title();
	}
	if (book0Type != NetworkLibraryBookItem::TYPE_ID || book1Type != NetworkLibraryBookItem::TYPE_ID) {
		return book0Type != NetworkLibraryBookItem::TYPE_ID;
	}

	NetworkLibraryBookItem &book0 = (NetworkLibraryBookItem &) *bookPtr0;
	NetworkLibraryBookItem &book1 = (NetworkLibraryBookItem &) *bookPtr1;

	/*if (book0.Index != book1.Index) {
		return book0.Index < book1.Index;
	}*/

	const bool book0HasSeriesName = !book0.seriesName().empty();
	const bool book1HasSeriesName = !book1.seriesName().empty();
	if (book0HasSeriesName && book1HasSeriesName) {
		const int comp = book0.seriesName().compare(book1.seriesName());
		if (comp != 0) {
			return comp < 0;
		} else {
			int diff = book0.indexInSeries() - book1.indexInSeries();
			if (diff != 0) {
				return diff < 0;
			}
		}
		return book0.title() < book1.title();
	}

	const std::string &book0Key = book0HasSeriesName ? book0.seriesName() : book0.title();
	const std::string &book1Key = book1HasSeriesName ? book1.seriesName() : book1.title();
	const int comp = book0Key.compare(book1Key);
	if (comp != 0) {
		return comp < 0;
	}
	return book1HasSeriesName;
}


NetworkAuthorComparator::NetworkAuthorComparator(const std::map<NetworkLibraryBookItem::AuthorData, unsigned int> &rates) : myRates(rates) {
}

bool NetworkAuthorComparator::operator () (const NetworkLibraryBookItem::AuthorData &author0, const NetworkLibraryBookItem::AuthorData &author1) {
	std::map<NetworkLibraryBookItem::AuthorData, unsigned int>::const_iterator it1 = myRates.find(author0);
	std::map<NetworkLibraryBookItem::AuthorData, unsigned int>::const_iterator it2 = myRates.find(author1);
	if (it1 == myRates.end() && it2 == myRates.end()) {
		return author0 < author1;
	}
	if (it1 == myRates.end()) {
		return false;
	}
	if (it2 == myRates.end()) {
		return true;
	}
	if (it1->second != it2->second) {
		return it1->second < it2->second;
	}
	return author0 < author1;
}

