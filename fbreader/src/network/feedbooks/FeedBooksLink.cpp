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

#include <cctype>

#include "FeedBooksLink.h"
#include "FeedBooksDataParser.h"
#include "../NetworkBookInfo.h"
#include "../XMLParserCurlData.h"

FeedBooksLink::FeedBooksLink() : NetworkLink("FeedBooks.Com", "feedbooks.com") {
}

shared_ptr<CurlData> FeedBooksLink::simpleSearchData(NetworkBookList &books, const std::string &pattern) {
	return new XMLParserCurlData(
		"http://feedbooks.com/books/search.xml?query=" + htmlEncode(pattern),
		new FeedBooksDataParser(books)
	);
}

shared_ptr<CurlData> FeedBooksLink::advancedSearchData(NetworkBookList &books, const std::string &title, const std::string &author, const std::string &series, const std::string &tag, const std::string &annotation) {
	// TODO: replace by advanced query
	return new XMLParserCurlData(
		"http://feedbooks.com/books/search.xml?query=" + htmlEncode(title),
		new FeedBooksDataParser(books)
	);
}
