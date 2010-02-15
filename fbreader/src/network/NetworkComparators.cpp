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

#include "NetworkComparators.h"

bool NetworkBookItemComparator::operator () (const shared_ptr<NetworkItem> &bookPtr0, const shared_ptr<NetworkItem> &bookPtr1) {
	const bool book0isABook =
		bookPtr0->isInstanceOf(NetworkBookItem::TYPE_ID);
	const bool book1isABook =
		bookPtr1->isInstanceOf(NetworkBookItem::TYPE_ID);

	if (!book0isABook && !book1isABook) {
		return bookPtr0->Title < bookPtr1->Title;
	}
	if (!book0isABook || !book1isABook) {
		return !book0isABook;
	}

	const NetworkBookItem &book0 = (NetworkBookItem &) *bookPtr0;
	const NetworkBookItem &book1 = (NetworkBookItem &) *bookPtr1;

	const std::vector<NetworkBookItem::AuthorData> &authors0 = book0.Authors;
	const std::vector<NetworkBookItem::AuthorData> &authors1 = book1.Authors;
	if (authors0.empty() && !authors1.empty()) {
		return true;
	}
	if (authors1.empty() && !authors0.empty()) {
		return false;
	}
	if (!authors0.empty() && !authors1.empty()) {
		if (authors0.front().SortKey != authors1.front().SortKey) {
			return authors0.front().SortKey < authors1.front().SortKey;
		}
	}

	/*if (book0.Index != book1.Index) {
		return book0.Index < book1.Index;
	}*/

	const bool book0HasSeriesTitle = !book0.SeriesTitle.empty();
	const bool book1HasSeriesTitle = !book1.SeriesTitle.empty();
	if (book0HasSeriesTitle && book1HasSeriesTitle) {
		const int comp = book0.SeriesTitle.compare(book1.SeriesTitle);
		if (comp != 0) {
			return comp < 0;
		} else {
			int diff = book0.IndexInSeries - book1.IndexInSeries;
			if (diff != 0) {
				return diff < 0;
			}
		}
		return book0.Title < book1.Title;
	}

	const std::string &book0Key = book0HasSeriesTitle ? book0.SeriesTitle : book0.Title;
	const std::string &book1Key = book1HasSeriesTitle ? book1.SeriesTitle : book1.Title;
	const int comp = book0Key.compare(book1Key);
	if (comp != 0) {
		return comp < 0;
	}
	return book1HasSeriesTitle;
}


NetworkAuthorComparator::NetworkAuthorComparator(const std::map<NetworkBookItem::AuthorData, unsigned int> &rates) : myRates(rates) {
}

bool NetworkAuthorComparator::operator () (const NetworkBookItem::AuthorData &author0, const NetworkBookItem::AuthorData &author1) {
	std::map<NetworkBookItem::AuthorData, unsigned int>::const_iterator it1 = myRates.find(author0);
	std::map<NetworkBookItem::AuthorData, unsigned int>::const_iterator it2 = myRates.find(author1);
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

