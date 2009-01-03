/*
 * Copyright (C) 2008-2009 Geometer Plus <contact@geometerplus.com>
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

RecentBooksPopupData::RecentBooksPopupData(FBReader &fbreader) : myFBReader(fbreader), myId(0) {
}

void RecentBooksPopupData::updateId() {
	++myId;
}

size_t RecentBooksPopupData::id() const {
	return myId;
}

size_t RecentBooksPopupData::count() const {
	return myFBReader.recentBooks().books().size();
}

const std::string RecentBooksPopupData::text(size_t index) {
	Books books = myFBReader.recentBooks().books();
	if (index >= books.size()) {
		return "";
	}
	const BookDescription &book = *books[index];
	return book.author()->displayName() + ". " + book.title();
}

void RecentBooksPopupData::run(size_t index) {
	Books books = myFBReader.recentBooks().books();
	if (index >= books.size()) {
		return;
	}
	myFBReader.openBook(books[index]);
	myFBReader.showBookTextView();
	myFBReader.refreshWindow();
}
