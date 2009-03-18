/*
 * Copyright (C) 2004-2009 Geometer Plus <contact@geometerplus.com>
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

#include <ZLStringUtil.h>
#include <ZLOptions.h>

#include "BookCollection.h"

const std::string GROUP = "LastOpenedBooks";
const std::string BOOK = "Book";

const size_t RecentBooks::MaxListSize = 10;

RecentBooks::RecentBooks() {
	for (size_t i = 0; i < MaxListSize; ++i) {
		std::string num = BOOK;
		ZLStringUtil::appendNumber(num, i);
		std::string name = ZLStringOption(ZLCategoryKey::STATE, GROUP, num, "").value();
		if (!name.empty()) {
			BookDescriptionPtr description = BookDescription::getDescription(name, false);
			if (!description.isNull()) {
				myBooks.push_back(description);
			}
		}
	}
}

RecentBooks::~RecentBooks() {
	const size_t size = std::min(MaxListSize, myBooks.size());
	for (size_t i = 0; i < size; ++i) {
		std::string num = BOOK;
		ZLStringUtil::appendNumber(num, i);
		ZLStringOption(ZLCategoryKey::STATE, GROUP, num, "").setValue(myBooks[i]->fileName());
	}
}

void RecentBooks::addBook(BookDescriptionPtr description) {
	if (description.isNull()) {
		return;
	}
	for (Books::iterator it = myBooks.begin(); it != myBooks.end(); ++it) {
		if ((*it)->fileName() == description->fileName()) {
			myBooks.erase(it);
			break;
		}
	}
	myBooks.insert(myBooks.begin(), description);
	if (myBooks.size() > MaxListSize) {
		myBooks.erase(myBooks.begin() + MaxListSize, myBooks.end());
	}
}
