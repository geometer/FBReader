/*
 * Copyright (C) 2004-2008 Geometer Plus <contact@geometerplus.com>
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

LastOpenedBooks::LastOpenedBooks() :
	MaxListSizeOption(ZLCategoryKey::STATE, GROUP, "MaxSize", 1, 100, 10) {

	const int size = MaxListSizeOption.value();
	for (int i = 0; i < size; ++i) {
		std::string num = BOOK;
		ZLStringUtil::appendNumber(num, i);
		std::string name = ZLStringOption(ZLCategoryKey::STATE, GROUP, num, "").value();
		if (!name.empty()) {
			BookDescriptionPtr description = BookDescription::getDescription(name);
			if (!description.isNull()) {
				myBooks.push_back(description);
			}
		}
	}
}

LastOpenedBooks::~LastOpenedBooks() {
	const int size = std::min(MaxListSizeOption.value(), (long)myBooks.size());
	for (int i = 0; i < size; ++i) {
		std::string num = BOOK;
		ZLStringUtil::appendNumber(num, i);
		ZLStringOption(ZLCategoryKey::STATE, GROUP, num, "").setValue(myBooks[i]->fileName());
	}
}

void LastOpenedBooks::addBook(const std::string &fileName) {
	for (Books::iterator it = myBooks.begin(); it != myBooks.end(); ++it) {
		if ((*it)->fileName() == fileName) {
			myBooks.erase(it);
			break;
		}
	}
	BookDescriptionPtr description = BookDescription::getDescription(fileName);
	if (!description.isNull()) {
		myBooks.insert(myBooks.begin(), description);
	}
	const size_t maxSize = MaxListSizeOption.value();
	if (myBooks.size() > maxSize) {
		myBooks.erase(myBooks.begin() + maxSize, myBooks.end());
	}
}
