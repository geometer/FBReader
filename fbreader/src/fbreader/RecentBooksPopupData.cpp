/*
 * Copyright (C) 2008-2010 Geometer Plus <contact@geometerplus.com>
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

#include "RecentBooksPopupData.h"
#include "FBReader.h"

#include "../library/Book.h"
#include "../library/Author.h"

RecentBooksPopupData::RecentBooksPopupData() : myId(0) {
}

void RecentBooksPopupData::updateId() {
	++myId;
}

size_t RecentBooksPopupData::id() const {
	return myId;
}

size_t RecentBooksPopupData::count() const {
	return Library::Instance().recentBooks().size();
}

const std::string RecentBooksPopupData::text(size_t index) {
	const BookList &books = Library::Instance().recentBooks();
	if (index >= books.size()) {
		return "";
	}
	const Book &book = *books[index];
	const AuthorList authors = book.authors();
	if (authors.empty()) {
		return book.title();
	} else {
		return authors[0]->name() + ". " + book.title();
	}
}

void RecentBooksPopupData::run(size_t index) {
	FBReader &fbreader = FBReader::Instance();
	const BookList &books = Library::Instance().recentBooks();
	if (index >= books.size()) {
		return;
	}
	fbreader.openBook(books[index]);
	fbreader.showBookTextView();
	fbreader.refreshWindow();
}
